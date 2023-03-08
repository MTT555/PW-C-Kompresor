compile:
	cc -o program main.c countCharacters.c cipher.c utils.c huffman.c
	./program tekst out

debug:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c
	./debug tekst out

gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c
	gdb gdbdebug tekst out

valgrind:
	cc -o valgrind main.c countCharacters.c cipher.c utils.c huffman.c
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind tekst out

ala:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c
	./debug ala.in out

alac:
	cc -o debug -DDEBUG main.c countCharacters.c cipher.c utils.c huffman.c
	./debug ala.in out --c

.PHONY: clean

clean:
	-rm program debug gdbdebug valgrind valgrind-out.txt out 2> /dev/null