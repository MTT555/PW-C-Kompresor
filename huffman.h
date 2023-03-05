#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include "countCharacters.h"
void huffman(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key,count **head);
void print_huffmann_tree(count **head,int *code,int comp_level,int top);
#endif //HUFFMAN_H
