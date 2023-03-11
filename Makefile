compile:
	cc -o program main.c countCharacters.c cipher.c utils.c huffman.c output.c
	./program tekst out

debug:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	./debug tekst out

gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	gdb gdbdebug tekst out

valgrind:
	cc -o valgrind main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	valgrind ./valgrind tekst out

valgrind_full:
	cc -o valgrind main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind tekst out

ab:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	./debug ab.in out

ala:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	./debug ala.in out

alac:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	./debug ala.in out -c

test_help:
	cc -o program main.c countCharacters.c cipher.c utils.c huffman.c output.c -Wall -pedantic
	./program -h

.PHONY: clean

clean:
	-rm program debug gdbdebug valgrind valgrind-out.txt out 2> /dev/null