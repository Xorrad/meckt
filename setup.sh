#!/bin/bash

OS_TYPE="$(uname -s)"

case "$OS_TYPE" in
    Linux*)
        echo "Preparing meckt on native Linux..."
        
        echo "Downloading submodules..."
        git submodule init
        git submodule update

        echo "Installing dependencies..."
        sudo apt update && sudo apt install -y libsfml-dev make g++

        echo "Patching ImGui..."
        sed -i '10s|.*|#include "../../imgui.h"|' vendor/includes/imgui/imgui/misc/cpp/imgui_stdlib.cpp

        echo "Meckt has been successfully initialized."
        ;;
    CYGWIN*|MINGW*|MSYS*)
        echo "Preparing meckt on Windows (Git Bash/Cygwin/MSYS2)..."
        
        echo "Downloading submodules..."
        git submodule init
        git submodule update

        echo "Installing dependencies..."
        curl -L -o w64devkit.exe https://github.com/skeeto/w64devkit/releases/download/v2.3.0/w64devkit-x64-2.3.0.7z.exe
        chmod 777 w64devkit.exe
        ./w64devkit.exe
        rm w64devkit.exe

        echo "Installing SFML..."
        curl -L -o sfml.zip https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
        mkdir vendor
        tar -xf sfml.zip -C vendor
        mv vendor/SFML-2.5.1 vendor/sfml
        rm sfml.zip

        echo "Patching ImGui..."
        sed -i '10s|.*|#include "../../imgui.h"|' vendor/includes/imgui/imgui/misc/cpp/imgui_stdlib.cpp

        echo "Meckt has been successfully initialized."
        ;;
    *)
        echo "Unknown OS: $OS_TYPE"
        ;;
esac
