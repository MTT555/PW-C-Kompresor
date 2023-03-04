compile:
	cc -o program main.c countCharacters.c
	./program tekst
debug:
	cc -o debug -DDEBUG main.c countCharacters.c
	./debug tekst
gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c countCharacters.c
	gdb gdbdebug 
