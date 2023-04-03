#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/**
Funkcja wyswietlajaca pomoc zawierajaca instrukcje do prawidlowej obslugi programu
    FILE *stream - strumien outputu, zazwyczaj stdout lub stderr
*/
void help(FILE *stream) {
    char help_message1[] = "\n---------------------------------- HUFFMAN COMPRESSOR HELPBOX ----------------------------------\n\n"
                           "Compressor & decompressor made by Adrian Chmiel & Mateusz Tyl\n\n"
                           "Usage: [program_name] [input_file] [output_file] [arguments]\n\n"
                           "program_name - location of the program itself\n"
                           "input_file - location of the file that contains the string that is supposed to be compressed\n"
                           "output_file - location of the file that is supposed to save the result of the program\n";
    char help_message2[] = "arguments - arguments that change the settings and behaviour of the program\n\n"
                           "[input_file] and [output_file] fields are mandatory!\n"
                           "[arguments] field is optional and lets you provide more than one argument\n\n"
                           "Possible arguments:\n"
                           "-h - displays this help message\n"
                           "-c - encrypts the entire output\n"
                           "-x - force compression\n"
                           "-d - force decompression\n\n"
                           "Related to compression level (if provided, program behaviour will be automatically changed to \"force compression\"):\n";
    char help_message3[] = "-o0 - input string will not be compressed at all (default)\n"
                           "-o1 - input string will go under 8-bit Huffmann compression\n"
                           "-o2 - input string will go under 12-bit Huffmann compression\n"
                           "-o3 - input string will go under 16-bit Huffmann compression\n\n"
                           "You should not provide more than one of the possible compression levels!\n"
                           "Arguments -x and -d are mutually exclusive! Both of them should not be provided simultaneously!\n";
    char help_message4[] = "In case more than a single value is given, only the first one will be valid!\n\n"
                           "Error codes:\n"
                           "0 - Program finished successfully\n"
                           "1 - Too few arguments have been provided\n"
                           "2 - Input file could not be opened\n"
                           "3 - Output file could not be opened\n"
                           "4 - Input file is empty\n"
                           "5 - Decompression has been forced but the input file could not be decompressed\n\n"
                           "------------------------------------------------------------------------------------------------\n\n";
    fprintf(stream, "%s%s%s%s", help_message1, help_message2, help_message3, help_message4);
}

/**
Funkcja sluzaca do analizy argumentow podanych przez uzytkownika na wejsciu
    int argc - ilosc argumentow
    char **argv - zawartosc tych argumentow
    bool *cipher, *comp, *decomp, int *compLevel - wartosci tych zmiennych przekazemy przez pointery do main
*/
void analyzeArgs(int argc, char **argv, bool *cipher, bool *comp, bool *decomp, int *compLevel) {
    int i;
    bool help_displayed = false, setCompLevel = false; /* zmienna zapobiegajaca wielokrotnemu wyswietlaniu helpboxa oraz wielokrotnemu zmienianiu stopnia kompresji */
	char compMode[7]; /* zmienna pomocnicza do przechowywania trybu kompresji */
	char progBehaviour[20]; /* zmienna pomocnicza do przechowywania zachowania programu (wymuszenie kompresji/dekompresji) */

    if(argc > 3) {
		for(i = 3; i < argc; i++)
			if(strcmp(argv[i], "-c") == 0) {
				*cipher = true; /* argument -c mowiacy, ze wynik dzialania programu ma zostac dodatkowo zaszyfrowany */
				fprintf(stderr, "%s: Output encryption has been enabled!\n", argv[0]);
			} else if(strcmp(argv[i], "-o0") == 0) { /* brak kompresji */
				if(setCompLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
				}
				else {
					*compLevel = 0;
					strcpy(compMode, "none");
					setCompLevel = true;
					*comp = true;
					strcpy(progBehaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
				}
			} else if(strcmp(argv[i], "-o1") == 0) { /* kompresja 8-bit */
				if(setCompLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
				}
				else {
					*compLevel = 8;
					strcpy(compMode, "8-bit");
					setCompLevel = true;
					*comp = true;
					strcpy(progBehaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
				}
			} else if(strcmp(argv[i], "-o2") == 0) { /* kompresja 12-bit */
				if(setCompLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
				}
				else {
					*compLevel = 12;
					strcpy(compMode, "12-bit");
					setCompLevel = true;
					*comp = true;
					strcpy(progBehaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
				}
			} else if(strcmp(argv[i], "-o3") == 0) { /* kompresja 16-bit */
				if(setCompLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
				}
				else {
					*compLevel = 16;
					strcpy(compMode, "16-bit");
					setCompLevel = true;
					*comp = true;
					strcpy(progBehaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
				}
			} else if(strcmp(argv[i], "-h") == 0) {
				if(!help_displayed) { /* wyswietlenie pomocy */
					help(stderr);
					help_displayed = true;
				}
			} else if(strcmp(argv[i], "-x") == 0) { /* wymuszenie kompresji */
				if(*comp || *decomp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], progBehaviour);
				else {
					*comp = true;
					strcpy(progBehaviour, "force compression");
					fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], progBehaviour);
				}
			} else if(strcmp(argv[i], "-d") == 0) { /* wymuszenie dekompresji */
				if(*comp || *decomp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], progBehaviour);
				else {
					*decomp = true;
					strcpy(progBehaviour, "force decompression");
					fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], progBehaviour);
				}
			} else /* pominiecie niezidentyfikowanych argumentow */
				fprintf(stderr, "%s: %s -> Unknown argument! (ignoring...)\n", argv[0], argv[i]);
	}
}

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
int fileIsGood(FILE *in, uchar xorCorrectValue, bool displayMsg) {
    /* Deklaracja potrzebnych zmiennych */
    uchar c, xor; /* zmienna do odczytania wyniku sumy kontrolnej */
    int i, eof; /* zmienna do sprawdzenia pozycji koncowej */
    const char *impossibleOutput = "Provided file cannot be decompressed since it is not a possible output of this compressor!\n";


    /* Sprawdzenie poprawnosci oznaczenia na poczatku pliku */
    fseek(in, 0, SEEK_END); /* pobranie pozycji koncowej */
    eof = ftell(in);
    fseek(in, 0, SEEK_SET);
    if(eof < 4) {
        fprintf(stderr, "%s", displayMsg ? impossibleOutput : "");
        return 4;
    }

    fread(&c, sizeof(char), 1, in);
    if(c != 'C') {
        fseek(in, 0, SEEK_SET);
        fprintf(stderr, "%s", displayMsg ? impossibleOutput : "");
        return 1;
    }
    fread(&c, sizeof(char), 1, in);
    if(c != 'T') {
        fseek(in, 0, SEEK_SET);
        fprintf(stderr, "%s", displayMsg ? impossibleOutput : "");
        return 1;
    }
    fread(&c, sizeof(char), 1, in);
    if(!(c & 8)) { /* sprawdzenie bitu oznaczajacego kompresje (8 == 0b00001000) */
        fseek(in, 0, SEEK_SET);
        fprintf(stderr, "%s", displayMsg ? impossibleOutput : "");
        return 2;
    }

    /* sprawdzanie sumy kontrolnej xor */
    fread(&xor, sizeof(char), 1, in);
    for(i = 4; i < eof; i ++) {
        fread(&c, sizeof(char), 1, in);
        xor ^= c;
    }
    
    fseek(in, 0, SEEK_SET); /* ustawienie strumienia z powrotem na poczatek przed zakonczeniem dzialania funkcji */
#ifdef DEBUG
    /* wyswietlenie wyliczonej sumy kontrolnej na stderr */
    if(displayMsg)
        fprintf(stderr, "Control sum XOR: %d\n", xor);
#endif
    
    if(xor == xorCorrectValue) { /* jezeli xory sie zgadzaja, to plik jest prawidlowy */
#ifdef DEBUG
        if(displayMsg)
            fprintf(stderr, "Provided file can be decompressed!\n");
#endif
        return 0;
    }
    
    if(displayMsg) /* jezeli plik uszkodzony */
        fprintf(stderr, "Provided file cannot be decompressed since it is corrupted!\n");
    return 3;
}