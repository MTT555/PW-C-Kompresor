cmake_minimum_required(VERSION 3.00)
project(huffman C)
include_directories(src)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_executable(compare compare.c)

add_executable(generate gen/gen.c)

add_executable(compressor src/main.c
    src/countCharacters.c src/countCharacters.h
    src/decompress.c src/decompress.h
    src/huffman.c src/huffman.h
    src/output.c src/output.h
    src/utils.c src/utils.h
    src/list.c src/list.h
    src/dtree.c src/dtree.h
    src/bits_analyze.c src/bits_analyze.h
    src/alloc.c src/alloc.h
    src/noCompress.c src/noCompress.h
)

add_executable(debug src/main.c
    src/countCharacters.c src/countCharacters.h
    src/decompress.c src/decompress.h
    src/huffman.c src/huffman.h
    src/output.c src/output.h
    src/utils.c src/utils.h
    src/list.c src/list.h
    src/dtree.c src/dtree.h
    src/bits_analyze.c src/bits_analyze.h
    src/alloc.c src/alloc.h
    src/noCompress.c src/noCompress.h
)
target_compile_definitions(debug PRIVATE -DDEBUG)