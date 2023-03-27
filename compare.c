#include <stdio.h>

/**
Program sluzacy do porownywania, czy plik przed kompresja jest taki sam, jak ten otrzymany po dekompresji 
    Pliki podajemy jako argv[1] oraz argv[2]
*/
int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Nieprawidlowa liczba argumentow! (argc != 3)\n");
        return -1;
    }
    
    FILE *fptr1 = fopen(argv[1], "rb");
    FILE *fptr2 = fopen(argv[2], "rb");

    if(fptr1 == NULL || fptr2 == NULL) {
        fprintf(stderr, "Nieudana proba otwarcia pliku!\n");
        return -2;
    }
    
    fseek(fptr1, 0, SEEK_END);
    fseek(fptr2, 0, SEEK_END);
    int i, n = ftell(fptr1) > ftell(fptr2) ? ftell(fptr2) : ftell(fptr1), x = 0;
    fseek(fptr1, 0, SEEK_SET);
    fseek(fptr2, 0, SEEK_SET);

    unsigned char c1, c2;
    for(i = 0; i < n; i++) {
        fread(&c1, sizeof(char), 1, fptr1);
        fread(&c2, sizeof(char), 1, fptr2);
        if(c1 != c2) {
            printf(stderr, "This is crazy! Character: %d, f1: %d, f2: %d\n", i, c1, c2);
            x = 1;
        }
    }

    if(!x)
        printf("Files are the exact same!\n");

    fclose(fptr1);
    fclose(fptr2);
    return 0;
}