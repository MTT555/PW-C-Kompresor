#ifndef UTILS_H
#define UTILS_H

// Definicja zmiennej bool
typedef enum {
    false, true
} bool;

void help(FILE *stream);

int fileIsGood(FILE *in, char xor_correct_value, bool displayMsg);

#endif //UTILS_H