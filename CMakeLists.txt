cmake_minimum_required(VERSION 3.00)
project(huffman C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_executable(compare compare.c)

add_executable(generate gen/gen.c)

add_executable(compressor main.c
    countCharacters.c countCharacters.h
    decompress.c decompress.h
    huffman.c huffman.h
    output.c output.h
    utils.c utils.h
    )
