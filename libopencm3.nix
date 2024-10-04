# SPDX-License-Identifier: Apache-2.0

{ lib
, stdenvNoCC
, fetchFromGitHub
, writeText
, python311
, gcc-arm-embedded-13
, targets ? [ ]
}:

stdenvNoCC.mkDerivation rec {
  pname = "libopencm3";
  version = "228f28fa66b672535663aa9aa4d12ede8b1a8fc5";
  src = fetchFromGitHub {
    owner = "libopencm3";
    repo = pname;
    rev = "228f28fa66b672535663aa9aa4d12ede8b1a8fc5";
    sha256 = "sha256-eqjOtvH+FwRaWqVSxUyK/gXtveEipLsyjd4jLzXbzbw=";
  };
  setupHook = writeText "setup-hook.sh" ''
    export OPENCM3_DIR="$1"
  '';
  buildInputs = [
    python311
    gcc-arm-embedded-13 # arm-gnu-toolchain-13.2.rel1
  ];
  devicesPatch = writeText "devices-patch.txt" ''
    # Device otherwise missing from OpenCM3
    stm32l4r5zi stm32l4 ROM=2048K RAM=256K RAM3=384K
  '';
  postPatch = ''
    patchShebangs --build scripts/irq2nvic_h

    # Apply the patch to devices.data
    cat ${devicesPatch} >> ld/devices.data
  '';
  dontConfigure = true;
  buildPhase = ''
    make ${if targets == [] then "lib" else "TARGETS='${lib.concatMapStrings (t: t + " ") targets}'"}
  '';
  installPhase = ''
    runHook preInstall

    mkdir -p $out
    cp -r include/ $out/
    cp -r ld/ $out/
    cp -r lib/ $out/
    cp -r mk/ $out/
    cp -r scripts/ $out/

    runHook postInstall
  '';
  dontStrip = true;
  noAuditTmpdir = true;

  meta = {
    description = "Open source ARM Cortex-M microcontroller library";
    homepage = "https://libopencm3.org/";
  };
}
