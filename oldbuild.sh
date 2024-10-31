#!/bin/bash

# Setup environment
export SDK_PATH="$(pwd)/sdk/flipperzero-firmware"

# Clone SDK if not present
if [ ! -d "$SDK_PATH" ]; then
    mkdir -p sdk
    cd sdk
    git clone https://github.com/flipperdevices/flipperzero-firmware.git
    cd ..
fi

# Install emscripten if not present
if ! command -v emcc &> /dev/null; then
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh
    cd ..
fi

# Compile toolchain to WebAssembly
emcc toolchain.cpp \
    -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_malloc", "_free"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MAXIMUM_MEMORY=512MB \
    -I"$SDK_PATH/firmware/targets/f7/inc" \
    -I"$SDK_PATH/firmware/targets/furi_hal_include" \
    -O3 \
    -o flipper-toolchain.js

# Copy files to dist
mkdir -p dist
cp flipper-toolchain.{js,wasm} dist/
cp index.html dist/