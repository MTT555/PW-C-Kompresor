#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Too few/too many arguments! (argc == %d != 2)\n", argc);
        return -1;
    }
	
    FILE *out = fopen(argv[1], "wb");

    if(out == NULL) {
        fprintf(stderr, "File could not be opened!");
        return -2;
    }

    int i, j;
    for(i = 0; i < 256; i++)
        for(j = 0; j < 256; j++)
            fprintf(out, "%c%c", (unsigned char)i, (unsigned char)j);
    fclose(out);
	return 0;
}