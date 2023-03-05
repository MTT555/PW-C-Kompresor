#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include "utils.h"

void huffman(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key);

#endif //HUFFMAN_H