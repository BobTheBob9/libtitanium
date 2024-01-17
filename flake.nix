{
    description = "The titanium game engine and general development library";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/23.05";
        futils.url = "github:numtide/flake-utils";
    };

    outputs = { self, nixpkgs, futils }:
        futils.lib.eachDefaultSystem( system: let pkgs = import nixpkgs { inherit system; };
            in {
                packages = {
                    default = pkgs.stdenv.mkDerivation {
                        name = "libtitanium";
                        src = ./.;

                        nativeBuildInputs = [
                            pkgs.cmake
                            pkgs.clang_16
                            #pkgs.emscripten
                        ];

                        buildInputs = [
                            pkgs.vulkan-headers
                            pkgs.vulkan-tools
                            pkgs.vulkan-validation-layers
                            pkgs.SDL2
                        ];
                    };
                };
            }
        );
}
