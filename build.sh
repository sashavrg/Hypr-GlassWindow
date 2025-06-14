#!/bin/bash
set -euo pipefail

BUILD_DIR=build

echo "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"

echo "Running CMake configuration..."
cmake -S . -B "$BUILD_DIR"

echo "Building glasswindow plugin..."
cmake --build "$BUILD_DIR" --target glasswindow -- -j$(nproc)

echo "Build complete."

# Uncomment the following to install the plugin system-wide (requires sudo)
# echo "Installing plugin..."
# sudo cmake --install "$BUILD_DIR"

# Or copy manually if you prefer:
# cp "$BUILD_DIR/glasswindow.so" ~/.config/hyprland/plugins/

echo "Done."
