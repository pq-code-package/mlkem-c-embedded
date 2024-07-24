# SPDX-License-Identifier: Apache-2.0

{ stdenvNoCC
, writeText
, esp-idf-esp32c3
, git
, python3Packages
, fetchPypi
}:
let
  version = "v5.2.2";

  pyclang = python3Packages.buildPythonPackage rec {
    pname = "pyclang";
    version = "0.4.2";

    src = fetchPypi {
      inherit pname version;
      sha256 = "sha256-vuDZ5yEhyDpCmkXoC+Gr2X5vMK5B46HnktcvBONjxXM=";
    };

    doCheck = false;
  };

  esp-idf-esp32c3' = (esp-idf-esp32c3.overrideAttrs (old: {
    propagatedBuildInputs = builtins.filter (p: p != git) old.propagatedBuildInputs ++ [ pyclang ];
    patches = [ ];
  })
  ).override {
    rev = version;
    sha256 = "sha256-I4YxxSGdQT8twkoFx3zmZhyLTSagmeLD2pygVfY/pEk=";
  };
in
stdenvNoCC.mkDerivation {
  pname = "esp-idf-esp32c3-lib";
  version = version;

  dontUnpack = true; # start with empt src
  dontConfigure = true;

  # set environment variables for downstreams
  setupHook = writeText "setup-hook.sh" ''
    export ESP_IDF_DIR="${esp-idf-esp32c3'}"
    export ESP_IDF_LIB="$1"
  '';

  buildInputs = [ esp-idf-esp32c3' git ];
  propagatedBuildInputs = [ esp-idf-esp32c3' ];

  buildPhase = ''
    cp --no-preserve=mode,ownership -r "$IDF_PATH/examples/get-started/hello_world" ./
    idf.py -C hello_world set-target esp32c3
    idf.py -C hello_world build
  '';
  installPhase = ''
    mkdir -p $out/esp32c3
    mv ./hello_world/build/esp-idf/* $out/esp32c3/
  '';
}
