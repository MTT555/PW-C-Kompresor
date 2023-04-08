#include <stdio.h>
#include <string.h>
#include "noCompress.h"
#include "utils.h"

void rewriteFile(FILE *input, FILE *output, int n, settings_t s) {
    char c;
    int i;
    uchar xor = (uchar)183; /* (183 = 0b10110111) */
    int cipherLen, cipherPos = 0;
    uchar xxxx[4] = "CTXX"; /* cztery bity poczatkowe (wyjasnione na koncu pliku huffman.c) */
    xxxx[2] = (uchar)40; /* zapalone bity szyfrowania i kompresji (40 == 0b00101000) */
    if(s.cipher) { /* jezeli mamy szyfrowac */
        cipherLen = (int)strlen((char *)s.cipherKey);
        fwrite(xxxx, sizeof(char), 4, output); /* to zapisujemy bity poczatkowe */
        for(i = 0; i < n; i++) {
            fread(&c, sizeof(char), 1, input);
            c += s.cipherKey[cipherPos % cipherLen]; /* szyfrujemy kazdy znak */
            cipherPos++;
            fwrite(&c, sizeof(char), 1, output); /* zapisujemy znaki do pliku wyjsciowego */
            xor ^= c; /* wyliczamy sume kontrolna XOR */
        }
        fseek(output, 3, SEEK_SET);
        fwrite(&xor, sizeof(char), 1, output); /* zapisanie wartosci sumy na czwartym bajcie pliku */
        fprintf(stderr, "File successfully encrypted!\n");
    } else { /* jezeli nie szyfrujemy, to zwyczajnie przepisujemy plik bez zmian */
        for(i = 0; i < n; i++) {
            fread(&c, sizeof(char), 1, input);
            fwrite(&c, sizeof(char), 1, output);
        }
        fprintf(stderr, "Due to chosen settings, file has been rewritten with no changes!\n");
    }
}

void decryptFile(FILE *input, FILE *output, int n, uchar *cipherKey) {
    char c;
    int i, cipherPos = 0;
    int cipherLength = (int)strlen((char *)cipherKey);

    for(i = 4; i < n; i++) { /* pominiecie czterech bajtow poczatkowych poswieconych na oznaczenie */
        fread(&c, sizeof(char), 1, input);
        c -= cipherKey[cipherPos % cipherLength]; /* odszyfrowanie */
        cipherPos++;
        fwrite(&c, sizeof(char), 1, output); /* zapisanie znakow do pliku wyjsciowego */
    }
    fprintf(stderr, "File successfully decrypted!\n");
}