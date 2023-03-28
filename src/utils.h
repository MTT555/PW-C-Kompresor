#ifndef UTILS_H
#define UTILS_H

#define CIPHER "Politechnika_Warszawska"

// Definicja zmiennej bool
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
        unsigned char buf;
        unsigned char out;
    } chars;
} pack_t;

void help(FILE *stream);

int fileIsGood(FILE *in, unsigned char xor_correct_value, bool displayMsg);

#endif //UTILS_H