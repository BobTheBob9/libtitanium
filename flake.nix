{
    description = "The titanium game engine and general development library";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/23.11";
        flake-utils.url = "github:numtide/flake-utils";
    };

    # TODO: check for TITANIUM_SDL here?

    outputs = { self, nixpkgs, flake-utils }:
        flake-utils.lib.eachDefaultSystem( system: let pkgs = import nixpkgs { inherit system; };
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
