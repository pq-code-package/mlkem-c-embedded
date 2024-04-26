# SPDX-License-Identifier: Apache-2.0

{ lib
, stdenv
, fetchFromGitHub
, python311
, gcc-arm-embedded-13
}:

stdenv.mkDerivation rec {
  pname = "libopencm3";
  version = "ec5aeba354ec273782e4441440fe9000b1c965e3";
  src = fetchFromGitHub {
    owner = "libopencm3";
    repo = pname;
    rev = "ec5aeba354ec273782e4441440fe9000b1c965e3";
    sha256 = "sha256-bgoMhOhBJZwPTa9gUH0vPSGZknDrb2mJZuFlCWNivYU=";
  };
  buildInputs = [
    python311
    gcc-arm-embedded-13 # arm-gnu-toolchain-13.2.rel1
  ];
  buildPhase = ''
    make lib
  '';
  installPhase = ''
    mkdir -p $out
    cp -r include/ $out/
    cp -r ld/ $out/
    cp -r lib/ $out/
    cp -r mk/ $out/
    cp --parent scripts/genlink.py $out/
  '';
  dontStrip = true;
  dontPatchELF = true;
  dontPatchShebangs = true;
  noAuditTmpdir = true;

  meta = with lib; {
    description = "Open source ARM Cortex-M microcontroller library";
    homepage = "https://libopencm3.org/";
  };
}
