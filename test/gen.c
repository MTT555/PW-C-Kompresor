#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
Instrukcja, jak uzywac
    cc gen.c
    ./a.out [nazwa_pliku_wyjsciowego] [ilosc znakow] [ilosc roznych znakow, ktore moga wystapic w outpucie]
*/

int main(int argc, char **argv) {
    if(!(argc == 3 || argc == 4))
        return 1;
    
	FILE *out = fopen(argv[1], "w");
    int n = atoi(argv[2]);
    int r = argc > 4 ? atoi(argv[3]) : 128;
    int i;
    for(i = 0; i < n; i++)
        fprintf(out, "%c", (unsigned char)(rand() % r));
    fclose(out);
	return 0;
}
