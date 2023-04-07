#ifndef ALLOC_H
#define ALLOC_H

#include "utils.h"

/**
Funkcja dokonujaca probe alokacji pamieci
    void **ptr - wskaznik, pod ktory chcemy zaalokowac pamiec
    int size - ilosc pamieci w bajtach
*/
bool tryMalloc(void **ptr, int size);

/**
Funkcja dokonujaca probe realokacji pamieci na wiekszy blok
    void **ptr - wskaznik, ktorego pamiec chcemy realokowac
    int size - ilosc pamieci w bajtach
*/
bool tryRealloc(void **ptr, int size);

#endif