#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "countCharacters.h"
#include "huffman.h"
#include "output.h"

static unsigned int cipher_pos = 0; // zmienna przechowujaca aktualna pozycje w szyfrze

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int compLevel - poziom kompresji podany w bitach (brak obslugi przypadku braku kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    unsigned char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    listCodes **head - glowa listy zawierajaca ilosci wystapien danych znakow
    unsigned char xor_start_value - poczatkowa wartosc bajtu do wykonania sumy kontrolnej
    pack_t *buffer - union pack uzyty wczesniej do zapisu slownika
    short pack_pos - pozycja ostatniego zajetego bitu w tym packu
*/
void compressedToFile(FILE *input, FILE *output, int compLevel, bool cipher, unsigned char *cipher_key, listCodes **head, unsigned char *xor, pack_t *buffer, short *pack_pos) {
    unsigned char c;
    int i, j;
    unsigned char ending = '\0'; // ilosc niezapisanych bitow konczacych plik
    long int end_pos = ftell(output); // zapisanie pozycji koncowej outputu
    int tempCode = 0, currentBits = 0;

    fseek(input, 0, SEEK_SET); // ustawienie kursora na poczatek inputu
    listCodes *iterator = NULL; // iterator po liscie kodow
    bool endingZero = false;

    /**
    Poczatek zapisu do pliku - oznaczenie pliku skompresowanego
        C, T - pierwsze litery nazwisk autorow tego kompresora
        F - 8 bitow zarezerwowane na pozniejsze dopisanie potrzebnych flag
        X - wynik sumy kontrolnej xor, ktora ma sprawdzac czy plik nie jest uszkodzony przy probie dekompresji
    */
    fseek(output, 0, SEEK_SET);
    fprintf(output, "CTFX");
    fseek(output, end_pos, SEEK_SET);

    /// zapisywanie skompresowanego tekstu
    fseek(input, 0, SEEK_END);
    long int inputEOF = ftell(input);
    fseek(input, 0, SEEK_SET);
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
                    for(j = 0; j < strlen(iterator->code); j++)
                        saveBitIntoPack(output, cipher, cipher_key, buffer, pack_pos, xor, iterator->code[j] == '1' ? 1 : 0);
                    break;
                }
                else
                    iterator = iterator->next;
            }
            tempCode = 0;
            currentBits = 0;
        } else if (currentBits >= compLevel) { // taki przypadek wystapi jedynie w kompresji 12-bit
            int temp = tempCode % 16;
            tempCode >>= 4;
            iterator = (*head); // ustawienie iteratora na poczatek przy kazdym nastepnym symbolu
            while (iterator != NULL) {
                if(iterator->character == tempCode) {
                    for(j = 0; j < strlen(iterator->code); j++)
                        saveBitIntoPack(output, cipher, cipher_key, buffer, pack_pos, xor, iterator->code[j] == '1' ? 1 : 0);
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
    // wyswietlenie ilosci aktualnie wykorzystanych bitow po przejsciu przez caly plik wejsciowy na stderr
    fprintf(stderr, "Entire input file passed - currently used bits in the pack: %d/16 (unused: %d)\n", *pack_pos, 16 - *pack_pos);
#endif
    // po przejsciu po calym pliku, w razie potrzeby tworzymy ostatni "niepelny" znak z pozostalych nadmiarowych zapisanych bitow
    if (*pack_pos <= 8) // jezeli mamy zapelnione mniej lub rowno 8 bitow
        ending = (unsigned char)(8 - *pack_pos);
    else if(*pack_pos <= 16 && *pack_pos > 8)
        ending = (unsigned char)(16 - *pack_pos);
    for(i = 0; i <= (int)ending + 8; i++) // dopychamy union packa dodatkowymi zerami, aby zapisac ostatni znak do pliku
        saveBitIntoPack(output, cipher, cipher_key, buffer, pack_pos, xor, 0);

    end_pos = ftell(output); // zapisanie pozycji koncowej

    /**
    Zapisywanie potrzebnych flag w znaku F na samym poczatku pliku
    Szablon bitowy: 0bKKSCCEEE
    K - sposob kompresji: 00 - brak, 01 - 8-bit, 10 - 12-bit, 11 - 16-bit
    S - szyfrowanie: 0 - nie, 1 - tak
    Z - zapisanie informacji, czy konieczne bedzie usuniecie nadmiarowego znaku \0 z konca pliku podczas dekompresji
    C - dodatkowe sprawdzenie, czy ten plik jest skompresowany: 0 - nie, 1 - tak
    E - ilosc niezapisanych bitow konczacych zapisana binarnie
    */
    fseek(output, 2, SEEK_SET); // powrot kursora w pliku do znaku F
    unsigned char flags = (unsigned char)0b00001000 | ending; // zapisanie pozycji koncowej do flag oraz informacji, ze plik jest skompresowany
    if(compLevel == 8) // zapis informacji o poziomie kompresji pliku
        flags |= (unsigned char)0b01000000;
    else if(compLevel == 12)
        flags |= (unsigned char)0b10000000;
    else if(compLevel == 16)
        flags |= (unsigned char)0b11000000;
    if(cipher)
        flags |= (unsigned char)0b00100000;
    if(endingZero)
        flags |= (unsigned char)0b00010000;
    fprintf(output, "%c", flags); // wydrukowanie pojedynczego znaku zawierajacego wszystkie flagi

    /// Suma kontrolna xor
#ifdef DEBUG
    // wyswietlenie wyliczonej sumy kontrolnej na stderr
    fprintf(stderr, "Control sum XOR: %d\n", *xor);
#endif
    fseek(output, 3, SEEK_SET); // ustawienie kursora na znaku X w celu zapisania otrzymanej wartosci
    fprintf(output, "%c", *xor);

    // Wyswietlenie wiadomosci koncowej
    fprintf(stderr, "File successfully compressed!\n");
#ifdef DEBUG
    if(input != stdin && output != stdout) {
        if(ftell(input) > end_pos)
            fprintf(stderr, "File size reduced by %.2f%%\n", 100 - 100 * (double)end_pos/ftell(input));
        else
            fprintf(stderr, "File size increased by %.2f%%!!!\n", 100 * (double)end_pos/ftell(input) - 100);
        fprintf(stderr, "Input: %ld, output: %ld\n", ftell(input), end_pos);
    }
#endif
}

/**
Funkcja wykonujaca bitowy zapis znaku na podstawie union pack
    FILE *output - plik wyjsciowy
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    unsigned char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    pack_t *pack - union pack, nak torym wykonujemy operacje bitowe
    short *pack_pos - pozycja ostatniego zajetego bitu w tym union packu
    short bit - wartosc bitu, ktora ma zostac nadana
*/
void saveBitIntoPack(FILE *output, bool cipher, unsigned char *cipher_key, pack_t *buffer, short *pack_pos, unsigned char *xor, short bit) {
    unsigned int cipher_len = strlen(cipher_key);
    if((*pack_pos) == 16) { // jezeli bufer jest pelen
        if(cipher) { // jezeli plik ma zostac zaszyfrowany
            buffer->chars.out += cipher_key[cipher_pos % cipher_len]; // dokonujemy szyfrowania znaku
            cipher_pos++;
        }
        fprintf(output, "%c", buffer->chars.out); // wydrukuj znak
        (*xor) ^= buffer->chars.out; // uwzglednienie znaku w sumie kontrolnej 
#ifdef DEBUG
        // wyswietlenie zapisanego znaku wraz z jego kodem na stderr
        fprintf(stderr, "Saved to file the according symbol: %c (code: %d)\n", buffer->chars.out, (int)buffer->chars.out);
#endif
        *pack_pos -= 8; // zmniejsz pozycje o 8 bitow
    }
    buffer->whole <<= 1; // przesuniecie wszystkich liczb o jeden w prawo
    buffer->whole += bit; // nadanie wartosci bitowi powstalemu po przesunieciu
    (*pack_pos)++; // aktualizacja pozycji
}