#!/bin/bash

set -e

mkdir -p build
cd build

# Указываем install-folder и build-folder — чтобы всё оставалось в build/
conan install .. \
    --output-folder=. \
    --build=missing

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .

cd bin
./main