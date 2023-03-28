#ifndef BITSANALYZE_H
#define BITSANALYZE_H

#include "huffman.h"
#include "dtree.h"

// Zmienna przechowujaca aktualny tryb analizy aktualnych bitow
typedef enum {
    dictRoad, // slownik - odczytywanie drogi
    dictWord, // slownik - odczytywanie slowa
    bitsToWords // przeksztalcanie skompresowanych bitow na finalny tekst
} mod_t;

void analyzeBits(FILE *output, unsigned char c, int compLevel, listCodes **list,
    short reduntantBits, bool reduntantZero, dnode_t **iterator, mode_t *mode,
    unsigned char *buffer, int *curBufSize, unsigned char *codeBuf, int *curCodeBufSize,
    int *bufPos, int *codeBufPos, int *currentBits, int *tempCode);

short returnBit(unsigned char c, int x);

#endif // BITSANALYZE_H