#ifndef ALLOC_H
#define ALLOC_H

#include "utils.h"

/**
Funkcja dokonujaca probe alokacji pamieci
    void **ptr - wskaznik, pod ktory chcemy zaalokowac pamiec
    unsigned long int size - ilosc pamieci w bajtach
*/
bool tryMalloc(void **ptr, unsigned long int size);

/**
Funkcja dokonujaca probe realokacji pamieci na wiekszy blok
    void **ptr - wskaznik, ktorego pamiec chcemy realokowac
    unsigned long int size - ilosc pamieci w bajtach
*/
bool tryRealloc(void **ptr, unsigned long int size);

/**
Funkcje zwracaja:
    true - alokacja/realokacja sie powiodla
    false - niepowodzenie
*/

#endif