.PHONY: clean
.SILENT: clean

all: compressor debug generate compare

test: test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13 test14 clean

compressor:
	cc -o $@ src/*.c -ansi -pedantic -Wall

debug:
	cc -o $@ -DDEBUG src/*.c -ansi -pedantic -Wall

generate:
	cc -o $@ gen/gen.c -ansi -pedantic -Wall

compare:
	cc -o $@ compare.c -ansi -pedantic -Wall

clean:
	rm -f compressor generate compare debug out comp decomp
	rm -fdr cmake_build/*

# Przypadek, gdy plik wejsciowy jest pusty
test1: compressor
	./$< test/empty.in out || [ $$? -eq 4 ]
	@echo "--- Test 1 passed successfully! ---"

# Proba dekompresji uszkodzonego pliku skompresowanego
test2: compressor
	./$< test/corrupted.in out -d || [ $$? -eq 5 ]
	@echo "--- Test 2 passed successfully! ---"

# Test kompresji i dekompresji calego tekstu Pana Tadeusza
test3: compressor
	./$< test/tadeusz.in comp
	./$< comp decomp
	cmp test/tadeusz.in decomp
	@echo "--- Test 3 passed successfully! ---"

# Test wielkosci ok. 1.5 MB z kompresja 16-bit i szyfrowaniem
test4: compressor
	./$< test/testmax.in comp -o3 -c
	./$< comp decomp
	cmp test/testmax.in decomp
	@echo "--- Test 4 passed successfully! ---"

# Test kompresji i dekompresji muzyki z szyfrowaniem
test5: compressor
	./$< test/music.mp3 comp -c
	./$< comp decomp
	cmp test/music.mp3 decomp
	@echo "--- Test 5 passed successfully! ---"

# Test kompresji i dekompresji zdjecia z szyfrowaniem
test6: compressor
	./$< test/xp.jpg comp -c
	./$< comp decomp
	cmp test/xp.jpg decomp
	@echo "--- Test 6 passed successfully! ---"

# Test wygenerowany przy pomocy gen/gen.c (kompresja 8-bit)
test7: compressor
	./$< test/1.in comp -o1 -c
	./$< comp decomp
	cmp test/1.in decomp
	@echo "--- Test 7 passed successfully! ---"

# Test wygenerowany przy pomocy gen/gen.c (kompresja 12-bit)
test8: compressor
	./$< test/2.in comp -o2 -c
	./$< comp decomp
	cmp test/2.in decomp
	@echo "--- Test 8 passed successfully! ---"

# Test wygenerowany przy pomocy gen/gen.c (kompresja 16-bit)
test9: compressor
	./$< test/3.in comp -o3 -c
	./$< comp decomp
	cmp test/3.in decomp
	@echo "--- Test 9 passed successfully! ---"

# Test na rozbudowane drzewo Huffmana
test10: compressor
	./$< test/testhufftree.in comp -o1 -c
	./$< comp decomp
	cmp test/testhufftree.in decomp
	@echo "--- Test 10 passed successfully! ---"

# Wymuszenie dekompresji dla pliku nieskompresowanego
test11: compressor
	./$< test/ala.in out -d || [ $$? -eq 5 ]
	@echo "--- Test 11 passed successfully! ---"

# Przypadek, gdy plik nie istnieje
test12: compressor
	./$< doesnt_exist.in out || [ $$? -eq 2 ]
	@echo "--- Test 12 passed successfully! ---"

# Test na brak kompresji i brak szyfrowania
test13: compressor
	./$< test/ala.in comp -o0
	cmp test/ala.in comp
	@echo "--- Test 13 passed successfully! ---"

# Test na brak kompresji z szyfrowaniem
test14: compressor
	./$< test/ala.in comp -o0 -c
	./$< comp decomp
	cmp test/ala.in decomp
	@echo "--- Test 14 passed successfully! ---"