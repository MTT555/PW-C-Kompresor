#ifndef OUTPUT_H
#define OUTPUT_H

/**
pack_t - typ sluzacy za bufer dla znakow laczacy wlasciwosci shorta tj. liczby calkowitej oraz chara tj. typu znakowego
    short whole - 16-bitowa zmienna do sprawnego wykonywania operacji bitowych na calym union
    char chars.buf - znak utworzony z bitow 0-7 sluzacych jako bufer
    char chars.out - znak utworzony z bitow 8-15 tworzacych faktyczny znak wyjsciowy
*/
typedef union pack {
    short whole;
    struct {
        char buf;
        char out;
    } chars;
} pack_t;

void compressedToFile(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key, listCodes **head, char *xor, pack_t *buffer, short *pack_pos);

void decompressedToFile(FILE *in, FILE *out, char *cipher_key);

void saveBitIntoPack(FILE *output, bool cipher, char *cipher_key, pack_t *buffer, short *pack_pos, char *xor, short bit);

#endif //OUTPUT_H