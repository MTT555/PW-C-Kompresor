#include "cipher.h"

/**
Funkcja szyfrujaca dany ciag znakow (argument -c)
    char *text - tekst, ktory chcemy zaszyfrowac
    int size - dlugosc tego tekstu
    const char *key - klucz szyfrujacy
    int key_size - dlugosc tego klucza
*/
void vigenere(char *text, int size, const char *key, int key_size) {
    int i;
    for(i = 0; i < size; i++)
        text[i] += key[i % key_size];
}
