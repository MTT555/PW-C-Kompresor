#include <stdio.h>
#include <string.h>
#include "noCompress.h"
#include "utils.h"

void rewriteFile(FILE *input, FILE *output, int n, settings_t s) {
    char c;
    int i;
    uchar xor = (uchar)183; /* (183 = 0b10110111) */
    uchar cipherKey[] = "Politechnika_Warszawska";
    int cipherPos = 0;
    int cipherLen = (int)strlen((char *)s.cipherKey);

    if(s.cipher) {
        fprintf(output, "CT%cX", (uchar)40); /* zapalone bity szyfrowania i kompresji (40 == 0b00101000) */
        for(i = 0; i < n; i++) {
            fread(&c, sizeof(char), 1, input);
            c += cipherKey[cipherPos % cipherLen];
            cipherPos++;
            fwrite(&c, sizeof(char), 1, output);
            xor ^= c;
        }
        fseek(output, 3, SEEK_SET);
        fwrite(&xor, sizeof(char), 1, output);
        fprintf(stderr, "Due to chosen settings, file has been rewritten with encryption but no compression!\n");
    } else {
        for(i = 0; i < n; i++) {
            fread(&c, sizeof(char), 1, input);
            fwrite(&c, sizeof(char), 1, output);
        }
        fprintf(stderr, "Due to chosen settings, file has been rewritten with no changes!\n");
    }
}