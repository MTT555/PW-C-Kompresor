#include <stdio.h>
#include <stdlib.h>

/**
Instrukcja, jak uzywac
    cc testmax.c
    ./a.out [nazwa_pliku_wyjsciowego]
Zwraca:
    0 - generowanie testu pomyslne
    1 - nieprawidlowa ilosc argumentow wejsciowych
    2 - nieudana proba otwarcia pliku
*/

int main(int argc, char **argv) {
    int i, j, k;
    FILE *out;

    if(argc != 2) {
        fprintf(stderr, "Too few/too many arguments! (argc == %d != 2)\n", argc);
        return 1;
    }
	
    out = fopen(argv[1], "wb");
    if(out == NULL) {
        fprintf(stderr, "File could not be opened!");
        return 2;
    }
    for(i = 0; i <= 12; i++)
        for(j = 0; j < 256; j++)
            for(k = 0; k < 256; k++)
                fprintf(out, "%c%c", (unsigned char)j, (unsigned char)k);
    fclose(out);
	return 0;
}
