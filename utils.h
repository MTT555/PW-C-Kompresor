#ifndef UTILS_H
#define UTILS_H

// Definicja zmiennej bool
typedef enum {
    false, true
} bool;

void help(char *program_name, FILE *stream);

bool fileIsGood(FILE *in, char xor_correct_value);

#endif //UTILS_H