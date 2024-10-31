#!/bin/bash

# Create project structure
mkdir -p src sdk

# Clone SDK if not present
if [ ! -d "sdk/flipper-sdk" ]; then
    cd sdk
    git clone --depth 1 https://github.com/flipperdevices/flipperzero-firmware.git flipper-sdk
    cd ..
fi

# Set SDK path for compiler
export SDK_PATH="$(pwd)/sdk/flipper-sdk"

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
emcc src/toolchain.cpp \
    -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_malloc", "_free"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MAXIMUM_MEMORY=512MB \
    -I"./sdk/flipper-sdk/firmware/targets/f7/inc" \
    -I"./sdk/flipper-sdk/firmware/targets/furi_hal_include" \
    -O3 \
    -o dist/flipper-toolchain.js

echo "Build complete! Check dist/ directory for output files."