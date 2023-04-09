#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
Instrukcja, jak uzywac
    cc gen.c
    ./a.out [nazwa_pliku_wyjsciowego] [ilosc znakow] [ilosc roznych znakow, ktore moga wystapic w outpucie] [seed]
Zwraca:
    0 - generowanie testu pomyslne
    1 - nieprawidlowa ilosc argumentow wejsciowych
    2 - nieudana proba otwarcia pliku
*/

int main(int argc, char **argv) {
    int i, n, r, seed;
    FILE *out;
    char fileName[256] = "../test/";
    
    if(!(argc == 4 || argc == 5)) {
        fprintf(stderr, "Too few/too many arguments! (argc == %d != 2)\n", argc);
        return 1;
    }
    
    strcat(fileName, argv[1]);
    out = fopen(fileName, "wb");
    if(out == NULL) {
        fprintf(stderr, "File could not be opened!\n");
        return 2;
    }

    n = atoi(argv[2]);
    r = argc > 3 ? atoi(argv[3]) : 128;
    seed = argc > 4 ? atoi(argv[4]) : time(NULL);
    srand(seed);

    for(i = 0; i < n; i++)
        fprintf(out, "%c", (unsigned char)(rand() % r));
    fclose(out);
	return 0;
}
