.PHONY: clean
.SILENT: clean

all: compressor debug generate compare

compressor:
	cc -o $@ src/*.c

debug:
	cc -o $@ -DDEBUG src/*.c -ansi -pedantic -Wall

generate:
	cc -o $@ gen/gen.c

compare:
	cc -o $@ compare.c

valgrind:
	cc -o valgrind src/*.c -Wall -pedantic -DDEBUG
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./valgrind test/1.in out >valogs 2>logs

clean:
	rm -f compressor generate compare debug
	rm -fdr cmake_build/*
	rm -f valgrind