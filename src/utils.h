#ifndef UTILS_H
#define UTILS_H

#define CIPHER "Politechnika_Warszawska"
#define uchar unsigned char
#define XOR 183
/* (183 = 0b10110111) */

/* Definicja zmiennej bool */
typedef enum {
    false, true
} bool;

/**
pack_t - typ sluzacy za bufer dla znakow laczacy wlasciwosci shorta tj. liczby calkowitej oraz chara tj. typu znakowego
    short whole - 16-bitowa zmienna do sprawnego wykonywania operacji bitowych na calym union
    char chars.buf - znak utworzony z bitow 0-7 sluzacych jako bufer
    char chars.out - znak utworzony z bitow 8-15 tworzacych faktyczny znak wyjsciowy
*/
typedef union pack {
    unsigned short whole;
    struct {
        uchar buf;
        uchar out;
    } chars;
} pack_t;

void help(FILE *stream);

void analyzeArgs(int argc, char **argv, bool *cipher, bool *comp, bool *decomp, int *compLevel);

int fileIsGood(FILE *in, uchar xor_correct_value, bool displayMsg);

#endif