#ifndef BITSANALYZE_H
#define BITSANALYZE_H

#include "huffman.h"
#include "dtree.h"
#include "utils.h"

/* Zmienna przechowujaca aktualny tryb analizy aktualnych bitow */
typedef enum {
    dictRoad, /* slownik - odczytywanie drogi tj. kodu */
    dictWord, /* slownik - odczytywanie slowa */
    bitsToWords /* przeksztalcanie skompresowanych bitow na finalny tekst */
} mod_t;

void analyzeBits(FILE *output, uchar c, int compLevel, listCodes_t **list,
    short redundantBits, bool redundantZero, dnode_t **iterator, mod_t *mode,
    uchar *buffer, int *curBufSize, uchar *codeBuf, int *curCodeBufSize,
    int *bufPos, int *codeBufPos, int *currentBits, int *tempCode);

short returnBit(uchar c, int x);

#endif