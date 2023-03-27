#include <stdio.h>

/**
Program sluzacy do porownywania, czy plik przed kompresja jest taki sam, jak ten otrzymany po dekompresji 
    Pliki podajemy jako argv[1] oraz argv[2]
*/
int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Too few/too many arguments! (argc == %d != 3)\n", argc);
        return -1;
    }
    
    FILE *fptr1 = fopen(argv[1], "rb");
    FILE *fptr2 = fopen(argv[2], "rb");

    if(fptr1 == NULL || fptr2 == NULL) {
        fprintf(stderr, "One of the files could not be opened!\n");
        return -2;
    }

    fseek(fptr1, 0, SEEK_END);
    fseek(fptr2, 0, SEEK_END);

    int n1 = ftell(fptr1);
    int n2 = ftell(fptr2);
    if(n1 != n2) {
        fclose(fptr1);
        fclose(fptr2);
        fprintf(stderr, "Different sizes of input files -> they cannot be the same!\n");
        return -3;
    }

    fseek(fptr1, 0, SEEK_SET);
    fseek(fptr2, 0, SEEK_SET);

    unsigned char c1, c2;
    int i, ifTheSame = 1;
    for(i = 0; i < n1; i++) {
        fread(&c1, sizeof(char), 1, fptr1);
        fread(&c2, sizeof(char), 1, fptr2);
        if(c1 != c2) {
            printf("Different chars! CharacterPos: %d, File1: %c (%d), File2: %c (%d)\n", i, c1, (int)c1, c2, (int)c2);
            ifTheSame = 0;
        }
    }

    if(ifTheSame)
        printf("Files are the exact same!\n");

    fclose(fptr1);
    fclose(fptr2);
    return 0;
}