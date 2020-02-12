#!/bin/sh

# Current path
CURRENT_PATH=$(pwd)

# Create build folder
rm -rf build
mkdir -p build
cd build

# Set toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-windows-mingw32.cmake

# Build
cmake --build .

# Go back
cd $CURRENT_PATH