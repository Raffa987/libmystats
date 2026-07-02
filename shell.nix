{ pkgs ? import <nixpkgs> { config = { allowUnfree = true; }; } }:

pkgs.mkShell {
    # Strumenti necessari per la compilazione
    nativeBuildInputs = with pkgs; [
        gcc
        pkg-config
        bear
    ];

    # Librerie richieste
    buildInputs = with pkgs; [
        gtk4
        gtk4-layer-shell
        cudaPackages.cuda_nvml_dev 
    ];

    # Variabili d'ambiente per il linker (NVIDIA/OpenGL)
    NIX_LDFLAGS = "-L/run/opengl-driver/lib -rpath /run/opengl-driver/lib";
}