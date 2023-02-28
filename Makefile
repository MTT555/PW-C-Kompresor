compile:
	cc -o program main.c
debug:
	cc -o debug -DDEBUG main.c
gdbdebug:
	cc -o gdbdebug -ggdb -DDEBUG main.c
