#!/bin/bash

if [[ $# -eq 1 && $1 == "clean" ]]; then
    rm -f compare compressor debug generate
    rm -fdr cmake_build
elif [[ $# -eq 0 ]]; then
    rm -f compare compressor debug generate
    rm -fdr cmake_build
    mkdir cmake_build
    cd cmake_build 2>/dev/null
    cmake ..
    make
    mv compare ../
    mv compressor ../
    mv debug ../
    mv generate ../
else
    echo "$0: Invalid usage!"
    echo ""
    echo "---------------------------------------------------------------"
    echo ""
    echo "Clean all of the CMake build files and compiled programs using:"
    echo "$0 clean"
    echo ""
    echo "Compile all programs using:"
    echo "$0"
    echo ""
    echo "---------------------------------------------------------------"
fi
