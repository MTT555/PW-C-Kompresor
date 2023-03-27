compile:
	cc -o compressor main.c countCharacters.c utils.c huffman.c output.c decompress.c

debug:
	cc -o compressor -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic

gen:
	cc -o generate gen/gen.clean

.PHONY: clean
.SILENT: clean

clean:
	rm -f compressor generate
	rm -fdr cmake_build/*