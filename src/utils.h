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

/* pack_t - typ sluzacy za bufer dla znakow laczacy wlasciwosci shorta tj. liczby calkowitej oraz chara tj. typu znakowego */
typedef union pack {
    unsigned short whole; /* 16-bitowa zmienna do sprawnego wykonywania operacji bitowych na calej unii */
    struct {
        uchar buf; /* znak utworzony z bitow 0-7 sluzacych jako bufer */
        uchar out; /* znak utworzony z bitow 8-15 tworzacych faktyczny znak wyjsciowy */
    } chars;
} pack_t;

/**
Funkcja wyswietlajaca pomoc zawierajaca instrukcje do prawidlowej obslugi programu
    FILE *stream - strumien outputu, zazwyczaj stdout lub stderr
*/
void help(FILE *stream);

/**
Funkcja sluzaca do analizy argumentow podanych przez uzytkownika na wejsciu
    int argc - ilosc argumentow
    char **argv - zawartosc tych argumentow
    bool *cipher, *comp, *decomp, int *compLevel - wartosci tych zmiennych przekazemy przez pointery do main
*/
void analyzeArgs(int argc, char **argv, bool *cipher, bool *comp, bool *decomp, int *compLevel);

/**
Funkcja sprawdzajaca podany plik pod wzgledem nadawania sie do dekompresji
1. Sprawdzenie oznaczenia na poczatku pliku
2. Sprawdzenie sumy kontrolnej
    FILE *in - plik wejsciowy, ktory ma zostac sprawdzony pod wzgledem poprawnosci
    uchar xorCorrectValue - wartosc startowa, od ktorej byly wykonywane sumy kontrolne podczas procesu kompresji
    bool displayMsg - wyswietlanie informacji o sprawdzanym pliku na stderr w przypadku, gdy nie spelnia wymogow do dekompresji (jesli == true)
Zwraca:
    0 - plik jest prawidlowy, mozna go dekompresowac
    1 - brak inicjalow na poczatku, plik nie pochodzi z kompresji
    2 - bity w bajcie flagowym wskazuja na inne pochodzenie niz kompresja
    3 - plik moze pochodzic z kompresji, lecz jest uszkodzony lub niepelny
    4 - plik jest zbyt krotki, aby mogl byc wynikiem dzialania tego kompresora
*/
int fileIsGood(FILE *in, uchar xor_correct_value, bool displayMsg);

#endif