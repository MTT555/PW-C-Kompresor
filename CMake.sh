#!/bin/bash

rm -f compare compressor debug generate
rm -fdr cmake_build
mkdir cmake_build
cd cmake_build
cmake ..
make
mv compare ../
mv compressor ../
mv debug ../
mv generate ../