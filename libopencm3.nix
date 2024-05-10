# SPDX-License-Identifier: Apache-2.0

{ lib
, stdenvNoCC
, fetchFromGitHub
, python311
, gcc-arm-embedded-13
, targets ? [ ]
}:

stdenvNoCC.mkDerivation rec {
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
  postPatch = ''
    patchShebangs --build scripts/irq2nvic_h
  '';
  dontConfigure = true;
  buildPhase = ''
    make ${if targets == [] then "lib" else "TARGETS=${lib.concatStrings targets}"}
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
