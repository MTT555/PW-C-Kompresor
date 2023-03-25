compile:
	cc -o program main.c countCharacters.c utils.c huffman.c output.c decompress.c
	./program ala.in out -d -o3 -c

debug:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug tekst out 2>logst

debug2:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug tekst out -o2

gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	gdb gdbdebug tekst out

valgrind:
	cc -o valgrind main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	valgrind ./valgrind tekst out

valgrind_full:
	cc -o valgrind main.c countCharacters.c  utils.c huffman.c output.c decompress.c -Wall -pedantic
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind tekst out

ala0:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug ala.in out -o0

ala0c:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug ala.in out -o0 -c

ala1:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug ala.in out

ala2:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug ala.in out -o2

ala3:
	cc -o debug -DDEBUG main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./debug ala.in out -o3

test_help:
	cc -o program main.c countCharacters.c utils.c huffman.c output.c decompress.c -Wall -pedantic
	./program -h

.PHONY: clean
.SILENT: clean

clean:
	-rm program debug gdbdebug valgrind valgrind-out.txt out out? test/a.out 2>/dev/null