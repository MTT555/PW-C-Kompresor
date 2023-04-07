#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "countCharacters.h"
#include "huffman.h"
#include "output.h"

void compressedToFile(FILE *input, FILE *output, int compLevel, bool cipher, uchar *cipherKey, listCodes_t **head, uchar *xor, pack_t *buffer, int *packPos) {
    /* Deklaracja potrzebnych zmiennych */
    uchar c;
    int i, j, inputEOF, outputEOF;
    int tempCode = 0, currentBits = 0; /* zmienne przechowujace tymczasowy kod znaku oraz ilosc obecnie odczytanych bitow */
    int cipherPos = 0; /* zmienna przechowujaca aktualna pozycje w szyfrze */
    uchar flags, ending = '\0'; /* ilosc niezapisanych bitow konczacych plik */
    listCodes_t *iterator = NULL; /* iterator po liscie kodow */
    bool endingZero = false;
    
    /* Zapisanie pozycji koncowych pliku wejsciowego oraz wyjsciowego */
    fseek(input, 0, SEEK_END);
    inputEOF = ftell(input);
    fseek(input, 0, SEEK_SET);

    /* Zapisywanie skompresowanego tekstu */
    for(i = 0; i <= inputEOF; i++) {
        if(i != inputEOF)
            fread(&c, sizeof(char), 1, input);
        else if((compLevel == 12 && (currentBits == 8 || currentBits == 4)) || (compLevel == 16 && currentBits == 8)) {
            c = '\0';
            if(!(compLevel == 12 && currentBits == 8))
                endingZero = true;
        }
        else
            break;
        
        currentBits += 8;
        tempCode <<= 8;
        tempCode += (int)c;
        if(currentBits == compLevel) {
            iterator = (*head);
            while (iterator != NULL) {
                if(iterator->character == tempCode) {
                    for(j = 0; j < (int)strlen((char *)(iterator->code)); j++)
                        saveBitIntoPack(output, cipher, cipherKey, &cipherPos, buffer, packPos, xor, iterator->code[j] == '1' ? 1 : 0);
                    break;
                }
                else
                    iterator = iterator->next;
            }
            tempCode = 0;
            currentBits = 0;
        } else if (currentBits >= compLevel) { /* taki przypadek wystapi jedynie w kompresji 12-bit */
            int temp = tempCode % 16;
            tempCode >>= 4;
            iterator = (*head); /* ustawienie iteratora na poczatek przy kazdym nastepnym symbolu */
            while (iterator != NULL) {
                if(iterator->character == tempCode) {
                    for(j = 0; j < (int)strlen((char *)(iterator->code)); j++)
                        saveBitIntoPack(output, cipher, cipherKey, &cipherPos, buffer, packPos, xor, iterator->code[j] == '1' ? 1 : 0);
                    break;
                }
                else
                    iterator = iterator->next;
            }
            tempCode = temp;
            currentBits = 4;
        }
    }

#ifdef DEBUG
    /* wyswietlenie ilosci aktualnie wykorzystanych bitow po przejsciu przez caly plik wejsciowy na stderr */
    fprintf(stderr, "Entire input file passed - currently used bits in the last union pack: %d/16 (unused: %d)\n", *packPos, 16 - *packPos);
#endif
    /* po przejsciu po calym pliku, w razie potrzeby tworzymy ostatni "niepelny" znak z pozostalych nadmiarowych zapisanych bitow */
    if (*packPos <= 8) /* jezeli mamy zapelnione mniej lub rowno 8 bitow */
        ending = (uchar)(8 - *packPos);
    else if(*packPos <= 16 && *packPos > 8)
        ending = (uchar)(16 - *packPos);
    for(i = 0; i <= (int)ending + 8; i++) /* dopychamy union packa dodatkowymi zerami, aby zapisac ostatni znak do pliku */
        saveBitIntoPack(output, cipher, cipherKey, &cipherPos, buffer, packPos, xor, 0);

    outputEOF = ftell(output); /* zapisanie pozycji koncowej */

    /**
    Zapisywanie potrzebnych flag w znaku F na samym poczatku pliku
    Szablon bitowy: 0bKKSZCEEE
    K - sposob kompresji: 00 - brak, 01 - 8-bit, 10 - 12-bit, 11 - 16-bit
    S - szyfrowanie: 0 - nie, 1 - tak
    Z - zapisanie informacji, czy konieczne bedzie usuniecie nadmiarowego znaku \0 z konca pliku podczas dekompresji
    C - dodatkowe sprawdzenie, czy ten plik jest skompresowany: 0 - nie, 1 - tak
    E - ilosc niezapisanych bitow konczacych zapisana binarnie
    */
    fseek(output, 2, SEEK_SET); /* powrot kursora w pliku do znaku F */
    flags = (uchar)(8 | ending); /* zapisanie pozycji koncowej do flag oraz informacji, ze plik jest skompresowany */
    if(compLevel == 8) /* zapis informacji o poziomie kompresji pliku */
        flags |= (uchar)64; /* (64 == 0b01000000) */
    else if(compLevel == 12)
        flags |= (uchar)128; /* (128 == 0b10000000) */
    else if(compLevel == 16)
        flags |= (uchar)192; /* (192 == 0b11000000) */
    if(cipher)
        flags |= (uchar)32; /* (32 == 0b00100000) */
    if(endingZero)
        flags |= (uchar)16; /* (16 == 0b00010000) */
    fwrite(&flags, sizeof(char), 1, output); /* wydrukowanie pojedynczego bajtu zawierajacego wszystkie flagi */

    /* Suma kontrolna XOR */
#ifdef DEBUG
    /* wyswietlenie wyliczonej sumy kontrolnej na stderr */
    fprintf(stderr, "Control sum XOR: %d\n", *xor);
#endif
    fseek(output, 3, SEEK_SET); /* ustawienie kursora na znaku X w celu zapisania otrzymanej wartosci */
    fwrite(xor, sizeof(char), 1, output);

    /* Wyswietlenie wiadomosci koncowej */
    fprintf(stderr, "File successfully compressed!\n");
#ifdef DEBUG
    if(inputEOF > outputEOF)
        fprintf(stderr, "File size reduced by %.2f%%!\n", 100 - 100 * (double)outputEOF / inputEOF);
    else
        fprintf(stderr, "File size increased by %.2f%%!\n", 100 * (double)outputEOF / inputEOF - 100);
    fprintf(stderr, "Input: %d, output: %d\n", inputEOF, outputEOF);
#endif
}

void saveBitIntoPack(FILE *output, bool cipher, uchar *cipherKey, int *cipherPos,
pack_t *buffer, int *packPos, uchar *xor, int bit) {
    int cipherLen = (int)strlen((char *)cipherKey);
    if((*packPos) == 16) { /* jezeli bufer jest pelen */
        if(cipher) { /* jezeli plik ma zostac zaszyfrowany */
            buffer->chars.out += cipherKey[(*cipherPos) % cipherLen]; /* dokonujemy szyfrowania znaku */
            cipherPos++;
        }
        fprintf(output, "%c", buffer->chars.out); /* wydrukuj znak */
        (*xor) ^= buffer->chars.out; /* uwzglednienie znaku w sumie kontrolnej  */
#ifdef CHARS
        /* wyswietlenie zapisanego znaku wraz z jego kodem na stderr */
        fprintf(stderr, "Saved to file the according character: %c (code: %d)\n", buffer->chars.out, (int)buffer->chars.out);
#endif
        *packPos -= 8; /* zmniejsz pozycje o 8 bitow */
    }
    buffer->whole <<= 1; /* przesuniecie wszystkich liczb o jeden w prawo */
    buffer->whole += bit; /* nadanie wartosci bitowi powstalemu po przesunieciu */
    (*packPos)++; /* aktualizacja pozycji */
}