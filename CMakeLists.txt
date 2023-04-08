cmake_minimum_required(VERSION 3.00)
project(huffman C)
include_directories(src)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")


# Executable targets
add_executable(compare compare.c)
add_executable(generate gen/gen.c)

file(GLOB SOURCE_FILES src/*.c src/*.h)
add_executable(compressor ${SOURCE_FILES})

add_executable(debug ${SOURCE_FILES})
target_compile_definitions(debug PRIVATE -DDEBUG)


# Test targets
add_custom_target(test
    DEPENDS test1 test2 test3 test4 test5 test6 test7 test8 test9
            test10 test11 test12 test13 test14
)

# Przypadek, gdy plik wejsciowy jest pusty
add_custom_target(test1
    COMMAND compressor test/empty.in out || [ $? -eq 4 ]
    && MESSAGE("--- Test 1 passed successfully! ---")
)

# Proba dekompresji uszkodzonego pliku skompresowanego
add_custom_target(test2
    COMMAND compressor test/corrupted.in out -d || [ $? -eq 5 ]
    && MESSAGE("--- Test 2 passed successfully! ---")
)

# Test kompresji i dekompresji calego tekstu Pana Tadeusza
add_custom_target(test3
    COMMAND compressor test/tadeusz.in comp
    && compressor comp decomp
    && cmp test/tadeusz.in decomp
    && MESSAGE("--- Test 3 passed successfully! ---")
)

# Test wielkosci ok. 1.5 MB z kompresja 16-bit i szyfrowaniem
add_custom_target(test4
    COMMAND compressor test/testmax.in comp -o3 -c
    && compressor comp decomp
    && cmp test/testmax.in decomp
    && MESSAGE("--- Test 4 passed successfully! ---")
)

# Test kompresji i dekompresji muzyki z szyfrowaniem
add_custom_target(test5
    COMMAND compressor test/music.mp3 comp -c
    && compressor comp decomp
    && cmp test/music.mp3 decomp
    && MESSAGE("--- Test 5 passed successfully! ---")
)

# Test kompresji i dekompresji zdjecia z szyfrowaniem
add_custom_target(test6
    COMMAND compressor test/xp.jpg comp -c
    && compressor comp decomp
    && cmp test/xp.jpg decomp
    && MESSAGE("--- Test 6 passed successfully! ---")
)

# Test wygenerowany przy pomocy gen/gen.c (kompresja 8-bit)
add_custom_target(test7
    COMMAND compressor test/1.in comp -o1 -c
    && compressor comp decomp
    && cmp test/1.in decomp
    && MESSAGE("--- Test 7 passed successfully! ---")
)

# Test wygenerowany przy pomocy gen/gen.c (kompresja 12-bit)
add_custom_target(test8
    COMMAND compressor test/2.in comp -o2 -c
    && compressor comp decomp
    && cmp test/2.in decomp
    && MESSAGE("--- Test 8 passed successfully! ---")
)

# Test wygenerowany przy pomocy gen/gen.c (kompresja 16-bit)
add_custom_target(test9
    COMMAND compressor test/3.in comp -o3 -c
    && compressor comp decomp
    && cmp test/3.in decomp
    && MESSAGE("--- Test 9 passed successfully! ---")
)

# Test na rozbudowane drzewo Huffmana
add_custom_target(test10
    COMMAND compressor test/testhufftree.in comp -o1 -c
    && compressor comp decomp
    && cmp test/testhufftree.in decomp
    && MESSAGE("--- Test 10 passed successfully! ---")
)

# Wymuszenie dekompresji dla pliku nieskompresowanego
add_custom_target(test11
    COMMAND compressor test/ala.in out -d || [ $? -eq 5 ]
    && MESSAGE("--- Test 11 passed successfully! ---")
)

# Przypadek, gdy plik nie istnieje
add_custom_target(test12
    COMMAND compressor doesnt_exist.in out || [ $? -eq 2 ]
    && MESSAGE("--- Test 12 passed successfully! ---")
)

# Test na brak kompresji i brak szyfrowania
add_custom_target(test13
    COMMAND compressor test/ala.in comp -o0
    && cmp test/ala.in comp
    && MESSAGE("--- Test 13 passed successfully! ---")
)

# Test na brak kompresji z szyfrowaniem
add_custom_target(test14
    COMMAND compressor test/ala.in comp -o0 -c
    && compressor comp decomp
	&& cmp test/ala.in decomp
    && MESSAGE("--- Test 14 passed successfully! ---")
)