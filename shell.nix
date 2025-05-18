{pkgs ? import <nixpkgs> {}}: let
in
  pkgs.mkShell {
    packages = [pkgs.python3 pkgs.poetry];
    buildInputs = [
      pkgs.platformio
      # Required for uploading to Arduino boards
      pkgs.avrdude
    ];
#    shellHook = ''
#    export PLATFORMIO_CORE_DIR=$PWD/.platformio
#    '';
  }
