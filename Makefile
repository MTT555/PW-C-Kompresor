.PHONY: clean
.SILENT: clean

all: compressor generate compare

compressor:
	cc -o $@ src/*.c

debug:
	cc -o compressor -DDEBUG src/*.c -Wall -pedantic

generate:
	cc -o $@ gen/gen.c

compare:
	cc -o $@ compare.c

clean:
	rm -f compressor generate compare debug
	rm -fdr cmake_build/*