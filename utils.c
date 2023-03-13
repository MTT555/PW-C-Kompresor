#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/**
Funkcja wyswietlajaca pomoc zawierajaca instrukcje do prawidlowej obslugi programu
    FILE *stream - strumien outputu, zazwyczaj stdout lub stderr
*/
void help(FILE *stream) {
    char help_message[] = "\n---------------------------------- HUFFMAN COMPRESSOR HELPBOX ----------------------------------\n\n"
                          "Compressor & decompressor made by Adrian Chmiel & Mateusz Tyl\n\n"
                          "Usage: [program_name] [input_file] [output_file] [arguments]\n\n"
                          "program_name - location of the program itself\n"
                          "input_file - location of the file that contains the string that is supposed to be compressed\n"
                          "output_file - location of the file that is supposed to save the result of the program\n"
                          "arguments - arguments that change the settings and behaviour of the program\n\n"
                          "[program_name], [input_file] and [output_file] fields are mandatory\n"
                          "[arguments] field is optional and lets you provide more than one argument\n\n"
                          "Possible arguments:\n"
                          "-h - displays this help message\n"
                          "-c - encrypts the entire output\n"
                          "-x - force compression\n"
                          "-d - force decompression\n\n"
                          "-DDEBUG - shows various variables during the runtime of the program in stderr\n\n"
                          "Related to compression level:\n"
                          "-o0 - input string will not be compressed at all (default)\n"
                          "-o1 - input string will go under 8-bit Huffmann compression\n"
                          "-o2 - input string will go under 12-bit Huffmann compression\n"
                          "-o3 - input string will go under 16-bit Huffmann compression\n\n"
                          "You should not provide more than one of the possible compression levels!\n"
                          "Arguments -x and -d are mutually exclusive! Both of them should not be provided simultaneously!\n"
                          "In case more than a single value is given, only the first one will be valid!\n\n"
                          "Error codes:\n"
                          "0 - Program finished successfully\n"
                          "1 - Too few arguments have been provided\n"
                          "2 - Input file could not be opened\n"
                          "3 - Output file could not be opened\n"
                          "4 - Input file is empty\n"
                          "5 - Decompression has been forced but the input file could not be decompressed\n\n"
                          "------------------------------------------------------------------------------------------------\n\n";
    fprintf(stream, "%s", help_message);
}

/**
Funkcja sprawdzajaca podany plik pod wzgledem nadawania sie do dekompresji
1. Sprawdzenie oznaczenia na poczatku pliku
2. Sprawdzenie sumy kontrolnej
    FILE *in - plik wejsciowy, ktory ma zostac sprawdzony pod wzgledem poprawnosci
    char xor_correct_value - wartosc startowa, od ktorej byly wykonywane sumy kontrolne podczas procesu kompresji
    bool displayMsg - wyswietlanie informacji o sprawdzanym pliku na stdout w przypadku, gdy nie spelnia wymogow do dekompresji (jesli == true)
Zwraca:
    0 - plik jest prawidlowy, mozna go dekompresowac
    1 - brak inicjalow na poczatku, plik nie pochodzi z kompresji
    2 - bity w bajcie flagowym wskazuja na inne pochodzenie niz kompresja
    3 - plik moze pochodzic z kompresji, lecz jest uszkodzony lub niepelny
*/
int fileIsGood(FILE *in, char xor_correct_value, bool displayMsg) {
    /// sprawdzenie poprawnosci oznaczenia na poczatku pliku
    fseek(in, 0, SEEK_END); // pobranie pozycji koncowej
    int end_pos = ftell(in);
    fseek(in, 0, SEEK_SET); // ustawienie strumienia pliku na poczatek
    char c;
    int i;

    if((c = fgetc(in)) != 'C' || (c = fgetc(in)) != 'T') {
        fseek(in, 0, SEEK_SET);
        if(displayMsg)
            printf("Provided file cannot be decompressed since it is not a possible output of this compressor!\n");
        return 1;
    }
    if(!((c = fgetc(in)) & 0b00011000)) {
        fseek(in, 0, SEEK_SET);
        if(displayMsg)
            printf("Provided file cannot be decompressed since it is not a possible output of this compressor!\n");
        return 2;
    }
    
    // po sprawdzeniu oznaczen zapisuje wszystkie potrzebne informacje z bajtu flagowego
    char xor = fgetc(in); // odczytanie wyniku sumy kontrolnej

    /// sprawdzanie sumy kontrolnej xor
    for(i = 4; i < end_pos; i ++) {
        c = fgetc(in);
        xor ^= c;
    }
    // ustawienie strumienia z powrotem na poczatek przed zakonczeniem dzialania funkcji   
    fseek(in, 0, SEEK_SET);
#ifdef DEBUG
    // wyswietlenie wyliczonej sumy kontrolnej na stderr
    fprintf(stderr, "Control sum XOR: %d\n", xor);
#endif

    // jezeli xory sie zgadzaja, to plik jest prawidlowy
    if(xor == xor_correct_value) {
#ifdef DEBUG
        fprintf(stderr, "Provided file can be decompressed! (XOR value: %d)\n", (int)xor);
#endif
        return 0;
    }
    
    // jezeli plik uszkodzony
    if(displayMsg)
        printf("Provided file cannot be decompressed since it is corrupted!\n");
    return 3;
}