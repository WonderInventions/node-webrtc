{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = nixpkgs.lib;
      in
      if lib.strings.hasSuffix "darwin" system then
        {
          devShells.default =
            let
              apple-sdk = pkgs.apple-sdk;
              clang = pkgs.llvmPackages_14.clang;
              clang-tools = pkgs.llvmPackages_14.clang-tools;
            in
            pkgs.mkShell {
              nativeBuildInputs =
                (with pkgs; [
                  cmake
                  darwin.apple_sdk.frameworks.AVFoundation
                  darwin.apple_sdk.frameworks.AppKit
                  darwin.apple_sdk.frameworks.Foundation
                  ninja
                  nodejs_20
                  pkg-config
                  zlib
                ])
                ++ [
                  clang
                  clang-tools
                ];
              # Build variables based on documentation from https://github.com/timniederhausen/gn-build/blob/01c96fd9981b111a3a028356284968acd77fa435/README.md
              shellHook = ''
                cat <<EOF > nix.gni
                is_clang=true
                use_lld=false
                clang_base_path="${clang}"
                clang_use_chrome_plugins=false
                mac_sdk_path="${apple-sdk}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
                EOF
              '';
            };
        }
      else
        {
          devShells.default =
            (pkgs.buildFHSEnv.override
              {
                stdenv = pkgs.gcc10Stdenv;
              }
              {
                name = "node-webrtc";
                targetPkgs =
                  pkgs:
                  (with pkgs; [
                    cmake
                    gcc10
                    ninja
                    nodejs_20
                    pkg-config
                    zlib
                  ]);
              }
            ).env;
        }
    );
}
