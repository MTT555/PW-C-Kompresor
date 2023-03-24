#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	FILE *out = fopen(argv[1], "wb");
    int i, j;
    for(i = 0; i < 256; i++)
        for(j = 0; j < 256; j++)
            fprintf(out, "%c%c", (unsigned char)i, (unsigned char)j);
    fclose(out);
	return 0;
}
