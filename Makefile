compile:
	cc -o program main.c countCharacters.c utils.c huffman.c output.c decompress.c
	./program ala.in out

debug:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug tekst out

gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	gdb gdbdebug tekst out

valgrind:
	cc -o valgrind main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	valgrind ./valgrind tekst out

valgrind_full:
	cc -o valgrind main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind tekst out

.PHONY: clean
.SILENT: clean

clean:
	-rm program debug gdbdebug valgrind valgrind-out.txt out out? test/a.out 2>/dev/null