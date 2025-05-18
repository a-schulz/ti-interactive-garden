{pkgs ? import <nixpkgs> {}}: let
in
  pkgs.mkShell {
    packages = [pkgs.python3 pkgs.poetry];
    buildInputs = [
      pkgs.platformio
      # optional: needed as a programmer i.e. for esp32
      pkgs.avrdude
    ];
    #shellHook = ''
    #export PLATFORMIO_CORE_DIR=$PWD/.platformio
    #'';
  }
