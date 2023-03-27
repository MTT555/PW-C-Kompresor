compressor:
	cc -o compressor src/*.c

debug:
	cc -o compressor -DDEBUG src/*.c -Wall -pedantic

generate:
	cc -o generate gen/gen.clean

compare:
	cc -o compare compare.c

.PHONY: clean
.SILENT: clean

clean:
	rm -f compressor generate compare
	rm -fdr cmake_build/*