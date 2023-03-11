#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/**
Funkcja wyswietlajaca pomoc zawierajaca instrukcje do prawidlowej obslugi programu
    char *program_name - nazwa programu, zazwyczaj argv[0]
    FILE *stream - strumien outputu, zazwyczaj stdout lub stderr
*/
void help(char *program_name, FILE *stream) {
    fprintf(stream, "Usage: %s [input_file] [output_file] [arguments]\n\n", program_name);
    char help_message[] = "input_file - location of the file that contains the string that is supposed to be compressed\n"
                          "output_file - location of the file that is supposed to save the result of the program\n"
                          "arguments - arguments that change the settings and behaviour of the program\n\n"
                          "[input_file] and [output_file] fields are mandatory\n"
                          "[arguments] field is optional and lets you provide more than one argument\n\n"
                          "Possible arguments:\n"
                          "-h - displays this help message\n"
                          "-c - encrypts the entire output\n"
                          "-v - shows the most significant variables during the runtime of the program in stdout\n"
                          "-x - force compression\n"
                          "-d - force decompression\n\n"
                          "Related to compression level:\n"
                          "-o0 - input string will not be compressed at all (default)\n"
                          "-o1 - input string will go under 8-bit Huffmann compression\n"
                          "-o2 - input string will go under 12-bit Huffmann compression\n"
                          "-o3 - input string will go under 16-bit Huffmann compression\n\n"
                          "You should not pass in arguments more than one of the possible compression levels!\n"
                          "In case more than a single value is provided, only the first one will be valid!\n";
    fprintf(stream, "%s", help_message);
}

/**
Funkcja sprawdzajaca, czy plik podany do dekompresji jest prawidlowy:
1. Sprawdzenie oznaczenia na poczatku pliku
2. Sprawdzenie sumy kontrolnej
    FILE *in - plik wejsciowy, ktory ma zostac sprawdzony pod wzgledem poprawnosci
    char xor_correct_value - wartosc startowa, od ktorej byly wykonywane sumy kontrolne podczas procesu kompresji
    Zwraca true, jezeli prawidlowy
    Zwraca false, jezeli nieprawidlowy, badz uszkodzony
*/
bool fileIsGood(FILE *in, char xor_correct_value) {
    // sprawdzenie poprawnosci oznaczenia na poczatku pliku
    fseek(in, 0, SEEK_SET); // ustawienie strumienia pliku na poczatek
    char c; // bufor
    bool isBad = false; // oznaczenie, ze plik jest nieprawidlowy, spowoduje zwrocenie false
    if((c = fgetc(in)) != 'C')
        isBad = true;
    if((c = fgetc(in)) != 'T')
        isBad = true;
    c = fgetc(in);
    if(!(c & 0b00011000))
        isBad = true;
    
    if(isBad) { // jezeli plik nieprawidlowy
        fseek(in, 0, SEEK_SET);
        return false;
    }
    
    // po sprawdzeniu oznaczen zapisuje wszystkie potrzebne informacje z bajtu flagowego
    char xor = fgetc(in); // odczytanie wyniku sumy kontrolnej
    c = fgetc(in);

    // zmienne odpowiedzialna za obsluge wystepowania EOF w skompresowanym pliku
    int eof_counter = 0;
    const int eof_limit = (int)c;

    // sprawdzanie sumy kontrolnej xor
    while(eof_counter <= eof_limit){
        c = fgetc(in);
        xor ^= c;
        if(c == EOF)
            eof_counter++;
    }
    xor ^= c; // poprawienie wartosci zmiennej po nadmiarowym xorze z EOF
    // ustawienie strumienia z powrotem na poczatek przed zakonczeniem dzialania funkcji   
    fseek(in, 0, SEEK_SET);
#ifdef DEBUG
    // wyswietlenie wyliczonej sumy kontrolnej na stderr
    fprintf(stderr, "Control sum XOR: %d\n", xor);
#endif
    if(xor == xor_correct_value)
        return true;
    return false;
}