#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cipher.h"
#include "utils.h"
#include "countCharacters.h"
#include "huffman.h"
#include "output.h"

static unsigned int cipher_pos = 0; // zmienna przechowujaca aktualna pozycje w szyfrze
static char eofile = (char)0; // ilosc wystapien znaku end of file podczas zapisu

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int comp_level - poziom kompresji podany w bitach (brak obslugi przypadku braku kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    count **head - glowa listy zawierajaca ilosci wystapien danych znakow
    char xor_start_value - poczatkowa wartosc bajtu do wykonania sumy kontrolnej
    pack_t *buffer - union pack uzyty wczesniej do zapisu slownika
    short pack_pos - pozycja ostatniego zajetego bitu w tym packu
*/
void compressedToFile(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key, listCodes **head, char *xor, pack_t *buffer, short *pack_pos) {
    char c;
    int i;
    char ending = (char)0; // ilosc niezapisanych bitow konczacych plik
    int end_pos = ftell(output); // zapisanie pozycji koncowej outputu
    unsigned int cipher_len = strlen(cipher_key); // dlugosc szyfru

    fseek(input, 0, SEEK_SET); // ustawienie kursora na poczatek inputu
    listCodes *iterator = NULL; // iterator po liscie kodow

    /**
    Poczatek zapisu do pliku - oznaczenie pliku skompresowanego
        C, T - pierwsze litery nazwisk autorow tego kompresora
        F - 8 bitow zarezerwowane na pozniejsze dopisanie potrzebnych flag
        X - wynik sumy kontrolnej xor, ktora ma sprawdzac czy plik nie jest uszkodzony przy probie dekompresji
        E - zapisanie ilosci wystapien znaku EOF przy zapisie
    Ze wzgledu na odrzucenie pomyslu z mozliwie najwiekszym zmniejszaniem pliku pozwalam sobie na tak dlugi kod
    */
    fseek(output, 0, SEEK_SET);
    fprintf(output, "CTFXE");
    fseek(output, end_pos, SEEK_SET);

    /// zapisywanie skompresowanego tekstu
    while((c = fgetc(input)) != EOF) { 
        iterator = (*head); // ustawienie iteratora na poczatek przy kazdym nastepnym symbolu
        while (iterator != NULL) {
            if(iterator->character == c) {
                for(i = 0; i < strlen(iterator->code); i++)
                    saveBitIntoPack(output, cipher, cipher_key, buffer, pack_pos, xor, iterator->code[i] == '1' ? 1 : 0);
                break;
            }
            else
                iterator = iterator->next;
        }
    }
#ifdef DEBUG
    // wyswietlenie ilosci aktualnie wykorzystanych bitow po przejsciu przez caly plik wejsciowy na stderr
    fprintf(stderr, "Entire input file passed - currently used bits in the pack: %d/16 (unused: %d)\n", *pack_pos, 16 - *pack_pos);
#endif
    // po przejsciu po calym pliku, w razie potrzeby tworzymy ostatni "niepelny" znak z pozostalych nadmiarowych zapisanych bitow
    if(*pack_pos != 16) {
        ending = (char)(16 - *pack_pos);
        buffer->whole <<= ending;
        if(cipher) { // jezeli plik ma zostac zaszyfrowany
            buffer->chars.out += cipher_key[cipher_pos % cipher_len]; // dokonujemy szyfrowania znaku
            cipher_pos++;
            buffer->chars.buf += cipher_key[cipher_pos % cipher_len];
            cipher_pos++;
        }
        fprintf(output, "%c%c", buffer->chars.out, buffer->chars.buf); // wydrukowanie ostatnich symboli
        *xor ^= buffer->chars.out; // oraz ich uwzglednienie w sumie kontrolnej
        *xor ^= buffer->chars.buf;
#ifdef DEBUG
        // wyswietlenie tych znakow wraz z kodami na stderr
        fprintf(stderr, "Printing last two symbols:\n"
                        "Printed symbol: %c (code: %d)\n", buffer->chars.out, (int)buffer->chars.out);
        fprintf(stderr, "Printed symbol: %c (code: %d)\n", buffer->chars.buf, (int)buffer->chars.buf);
        fprintf(stderr, "EOF amount: %d\n", (int)eofile);
#endif
    }
    end_pos = ftell(output); // zapisanie pozycji koncowej

    /**
    Zapisywanie potrzebnych flag w znaku F na samym poczatku pliku
    Szablon bitowy: 0bKKSCCEEE
    K - sposob kompresji: 00 - brak, 01 - 8-bit, 10 - 12-bit, 11 - 16-bit
    S - szyfrowanie: 0 - nie, 1 - tak
    C - dodatkowe sprawdzenie, czy ten plik jest skompresowany: 11 - tak, cokolwiek innego - nie
    E - ilosc niezapisanych bitow konczacych zapisana binarnie
    */
    fseek(output, 2, SEEK_SET); // powrot kursora w pliku do znaku F
    char flags = 0b00011000 | ending; // zapisanie pozycji koncowej do flag oraz informacji, ze plik jest skompresowany
    if(comp_level == 8) // zapis informacji o poziomie kompresji pliku
        flags |= 0b01000000;
    else if(comp_level == 12)
        flags |= 0b10000000;
    else if(comp_level == 16)
        flags |= 0b11000000;
    if(cipher)
        flags |= 0b00100000;
    fprintf(output, "%c", flags); // wydrukowanie pojedynczego znaku zawierajacego wszystkie flagi
    fseek(output, 4, SEEK_SET); // ustawienie kursora na znaku E
    fprintf(output, "%c", eofile); // zapisanie ilosci wystapien EOF

    /// Suma kontrolna xor
    fseek(output, 5, SEEK_SET); // ustawienie kursora pierwszym znaku zapisu slownika
    while((c = fgetc(output)) != EOF)
        *xor ^= c;
#ifdef DEBUG
    // wyswietlenie wyliczonej sumy kontrolnej na stderr
    fprintf(stderr, "Control sum XOR: %d\n", *xor);
#endif
    fseek(output, 3, SEEK_SET); // ustawienie kursora na znaku X w celu zapisania otrzymanej wartosci
    fprintf(output, "%c", *xor);

    // Wyswietlenie wiadomosci koncowej
    printf("File successfully compressed!");
    if(ftell(input) > end_pos)
        printf(" (file size reduced by %.2f%%)\n", 100 - 100 * ((double)end_pos)/ftell(input));
    else {
#ifdef DEBUG
        printf(" (file size increased by %.2f%%!!!)\n", 100 * ((double)end_pos)/ftell(input) - 100);
#endif
        printf("\n");
    }
}

/**
Funkcja wykonujaca zapis do pliku tekstu po dekompresji
    FILE *input - plik wejsciowy zawierajacy tekst skompresowany
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany tekst po dekompresji
    char *cipher_key - klucz szyfrowania (odszyfrowanie sie nie powiedzie, jezeli bedzie rozny od klucza uzytego podczas kompresji)
*/
void decompressedToFile(FILE *in, FILE *out, char *cipher_key) {
    unsigned int cipher_pos = 0; // zmienna przechowujaca aktualna pozycje w szyfrze
    unsigned int cipher_len = strlen(cipher_key); // dlugosc szyfru
}

/**
Funkcja wykonujaca bitowy zapis znaku na podstawie union pack
    FILE *output - plik wyjsciowy
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    pack_t *pack - union pack, nak torym wykonujemy operacje bitowe
    short *pack_pos - pozycja ostatniego zajetego bitu w tym union packu
    short bit - wartosc bitu, ktora ma zostac nadana
*/
void saveBitIntoPack(FILE *output, bool cipher, char *cipher_key, pack_t *buffer, short *pack_pos, char *xor, short bit) {
    unsigned int cipher_len = strlen(cipher_key);
    if((*pack_pos) == 16) { // jezeli bufer jest pelen
        if(cipher) { // jezeli plik ma zostac zaszyfrowany
            buffer->chars.out += cipher_key[cipher_pos % cipher_len]; // dokonujemy szyfrowania znaku
            cipher_pos++;
        }
        fprintf(output, "%c", buffer->chars.out); // wydrukuj znak
        (*xor) ^= buffer->chars.out; // uwzglednienie znaku w sumie kontrolnej 
        if(buffer->chars.out == EOF)
            eofile++;
#ifdef DEBUG
        // wyswietlenie zapisanego znaku wraz z jego kodem na stderr
        //fprintf(stderr, "Saved to file the according symbol: %c (code: %d)\n", buffer->chars.out, (int)buffer->chars.out);
#endif
        *pack_pos -= 8; // zmniejsz pozycje o 8 bitow
    }
    buffer->whole <<= 1; // przesuniecie wszystkich liczb o jeden w prawo
    buffer->whole += bit; // nadanie wartosci bitowi powstalemu po przesunieciu
    (*pack_pos)++; // aktualizacja pozycji
}