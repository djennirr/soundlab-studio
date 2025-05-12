#!/bin/bash

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..
make

cd bin

if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Запуск с lldb на macOS..."
    lldb ./main
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Запуск с gdb на Ubuntu..."
    gdb ./main
else
    echo "Неизвестная операционная система. Используйте macOS или Ubuntu.)))))))))))))))))))))"
    exit 1
fi