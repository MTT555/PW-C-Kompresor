cmake_minimum_required(VERSION 3.00)
project(huffman C)
include_directories(src)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_executable(compare compare.c)

add_executable(generate gen/gen.c)

file(GLOB SOURCE_FILES src/*.c src/*.h)

add_executable(compressor ${SOURCE_FILES})

add_executable(debug ${SOURCE_FILES})
target_compile_definitions(debug PRIVATE -DDEBUG)