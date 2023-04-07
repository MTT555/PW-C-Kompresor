#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "utils.h"

bool tryMalloc(void **ptr, int size) {
    *ptr = malloc(size);
    if(*ptr == NULL) { /* sprawdzam, czy alokacja pamieci sie powiodla */
#ifdef DEBUG
        fprintf(stderr, "Failed memory allocation!\n");
#endif
        return false;
    }
    return true;
}

bool tryRealloc(void **ptr, int size) {
    if(size < sizeof(*ptr)) { /* sprawdzam, czy blok wyjsciowy nie bedzie mniejszy od poczatkowego */
#ifdef DEBUG
        fprintf(stderr, "Attempt of reallocing to smaller block of memory - terminating!\n");
#endif
        return false;
    }

    *ptr = realloc(*ptr, size);

    if(*ptr == NULL) { /* sprawdzam, czy alokacja pamieci sie powiodla */
#ifdef DEBUG
        fprintf(stderr, "Failed memory reallocation!\n");
#endif
        return false;
    }
    return true;
}