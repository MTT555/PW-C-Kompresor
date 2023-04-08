.PHONY: clean
.SILENT: clean

all: compressor debug generate compare

test: test1 test2 test3 test4 test5 test6 test7 test8 test9 test10

compressor:
	cc -o $@ src/*.c -ansi -pedantic -Wall

debug:
	cc -o $@ -DDEBUG src/*.c -ansi -pedantic -Wall

generate:
	cc -o $@ gen/gen.c -ansi -pedantic -Wall

compare:
	cc -o $@ compare.c -ansi -pedantic -Wall

valgrind:
	cc -o valgrind src/*.c -Wall -pedantic -DDEBUG
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind test/1.in out >valogs 2>logs

clean:
	rm -f compressor generate compare debug out comp decomp
	rm -fdr cmake_build/*
	rm -f valgrind

# Przypadek, gdy plik wejsciowy jest pusty
test1: compressor
	./$< test/empty.in out || [ $$? -eq 3 ]
	@echo "--- Test 1 passed successfully! ---"

# Proba dekompresji uszkodzonego pliku skompresowanego
test2: compressor
	./$< test/corrupted.in out -d || [ $$? -eq 5 ]
	@echo "--- Test 2 passed successfully! ---"

# Test kompresji calego tekstu Pana Tadeusza
test3: compressor
	./$< test/tadeusz.in comp
	./$< comp decomp
	cmp test/tadeusz.in decomp
	@echo "--- Test 3 passed successfully! ---"

# Test wielkosci ok. 1.5 MB z kompresja 16-bit
test4: compressor
	./$< test/testmax.in comp -o3 -c
	./$< comp decomp
	cmp test/testmax.in decomp
	@echo "--- Test 4 passed successfully! ---"

# Test kompresji i szyfrowania muzyki
test5: compressor
	./$< test/music.mp3 comp -c
	./$< comp decomp
	cmp test/music.mp3 decomp
	@echo "--- Test 5 passed successfully! ---"

# Test kompresji i szyfrowania zdjecia
test6: compressor
	./$< test/xp.jpg comp -c
	./$< comp decomp
	cmp test/xp.jpg decomp
	@echo "--- Test 6 passed successfully! ---"

# Test wygenerowany przy pomocy gen/gen.c
test7: compressor
	./$< test/1.in comp -o2 -c
	./$< comp decomp
	cmp test/1.in decomp
	@echo "--- Test 7 passed successfully! ---"

# Test na rozbudowane drzewo Huffmana
test8: compressor
	./$< test/testhufftree.in comp -o1 -c
	./$< comp decomp
	cmp testhufftree.in decomp
	@echo "--- Test 8 passed successfully! ---"

# Wymuszenie dekompresji dla pliku nieskompresowanego
test9: compressor
	./$< test/ala.in out -d || [ $$? -eq 5 ]
	@echo "--- Test 9 passed successfully! ---"

# Przypadek, gdy plik nie istnieje
test10: compressor
	./$< doesnt_exist.in out || [ $$? -eq 2 ]
	@echo "--- Test 10 passed successfully! ---"