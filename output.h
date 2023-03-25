#ifndef OUTPUT_H
#define OUTPUT_H

void compressedToFile(FILE *input, FILE *output, int comp_level, bool cipher, unsigned char *cipher_key, listCodes **head, unsigned char *xor, pack_t *buffer, short *pack_pos);

void decompressedToFile(FILE *in, FILE *out, unsigned char *cipher_key);

void saveBitIntoPack(FILE *output, bool cipher, unsigned char *cipher_key, pack_t *buffer, short *pack_pos, unsigned char *xor, short bit);

#endif //OUTPUT_H