#ifndef OUTPUT_H
#define OUTPUT_H

#include "utils.h"
#include "list.h"

void compressedToFile(FILE *input, FILE *output, int compLevel, bool cipher,
    uchar *cipher_key, listCodes_t **head, uchar *xor, pack_t *buffer, int *pack_pos);

void decompressedToFile(FILE *in, FILE *out, uchar *cipher_key);

void saveBitIntoPack(FILE *output, bool cipher, uchar *cipherKey, int *cipherPos,
    pack_t *buffer, int *packPos, uchar *xor, int bit);

#endif