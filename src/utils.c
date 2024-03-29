#include <stdio.h>
#include <string.h>
#include "utils.h"

void help(FILE *stream) {
    /* podzial na 5 tablic w celu uzyskania pelnej zgodnosci ze standardem ANSII */
    char *helpMsg1 = "\n---------------------------------- HUFFMAN COMPRESSOR HELPBOX ----------------------------------\n\n"
                     "Compressor & decompressor made by Adrian Chmiel & Mateusz Tyl\n\n"
                     "Usage: [program_name] [input_file] [output_file] [arguments]\n\n"
                     "program_name - location of the program itself\n"
                     "input_file - location of the file that contains the string that is supposed to be compressed\n"
                     "output_file - location of the file that is supposed to save the result of the program\n";
    char *helpMsg2 = "arguments - arguments that change the settings and behaviour of the program\n\n"
                     "[input_file] and [output_file] fields are mandatory!\n"
                     "[arguments] field is optional and lets you provide more than one argument\n\n"
                     "Possible arguments:\n"
                     "-h - displays this help message\n"
                     "-x - force compression\n"
                     "-d - force decompression\n\n"
                     "-c \"cipher\" - encrypts/decrypts the entire output using the given cipher (max length: 4096)\n"
                     "If no custom cipher is provided, \"Politechnika_Warszawska\" is used by default)\n";
    char *helpMsg3 = "Important! If the cipher used during compression doesn't match with the one used during decompression,\n"
                     "the output will be highly inaccurate!\n\n"
                     "Related to compression level (if provided, program behaviour will be automatically changed to \"force compression\"):\n"
                     "-o0 - input string will not be compressed at all (default)\n"
                     "-o1 - input string will go under 8-bit Huffmann compression\n"
                     "-o2 - input string will go under 12-bit Huffmann compression\n"
                     "-o3 - input string will go under 16-bit Huffmann compression\n\n";
    char *helpMsg4 = "You should not provide more than one of the possible compression levels!\n"
                     "Arguments -x and -d are mutually exclusive! Both of them should not be provided simultaneously!\n"
                     "In case more than a single value is given, only the first one will be valid!\n\n"
                     "Error codes:\n"
                     "0 - Program finished successfully\n"
                     "1 - Too few arguments have been provided\n"
                     "2 - Input file could not be opened\n"
                     "3 - Output file could not be opened\n"
                     "4 - Input file is empty\n";
    char *helpMsg5 = "5 - Decompression has been forced but the input file could not be decompressed\n"
                     "6 - Memory allocation/reallocation failure\n"
                     "7 - Cipher provided during the decompression is not the same as the one given during the compression\n\n"
                     "------------------------------------------------------------------------------------------------\n\n";
    fprintf(stream, "%s%s%s%s%s", helpMsg1, helpMsg2, helpMsg3, helpMsg4, helpMsg5);
}

void analyzeArgs(int argc, char **argv, settings_t *s) {
    int i, j, cipherLen;
    bool help_displayed = false, setCompLevel = false, setCipher = false; /* zmienne zapobiegajace wielokrotnemu ustawianiu tych samych opcji */
	char compMode[7]; /* zmienna pomocnicza do przechowywania trybu kompresji */
	char progBehaviour[20]; /* zmienna pomocnicza do przechowywania zachowania programu (wymuszenie kompresji/dekompresji) */

    for(i = 3; i < argc; i++)
        if(strcmp(argv[i], "-c") == 0) { /* argument -c mowiacy, ze wynik dzialania programu ma zostac dodatkowo zaszyfrowany */
            if(setCipher)
                fprintf(stderr, "%s: %s -> Output encryption has already been enabled! (ignoring...)\n", argv[0], argv[i]);
            else {
                s->cipher = true;
                if(i + 1 != argc && argv[i + 1][0] != '-') {
                    cipherLen = (int)strlen(argv[i + 1]);
                    if(cipherLen > 4096) /* sprawdzenie limitu na dlugosc szyfru */
                        fprintf(stderr, "%s: Provided cipher is too long! Changing the cipher to default...\n", argv[0]);
                    else {
                        for(j = 0; j < cipherLen; j++) /* przepisywanie ustalonego szyfru do ustawien */
                            s->cipherKey[j] = (uchar)(argv[i + 1][j]);
                        s->cipherKey[j] = '\0';
                        i++;
                    }
                }
                setCipher = true;
                fprintf(stderr, "%s: Output encryption/decryption has been enabled with cipher: \"%s\"!\n", argv[0], s->cipherKey);
            }
        } else if(strcmp(argv[i], "-o0") == 0) { /* brak kompresji */
            if(setCompLevel)
                fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
            else {
                s->compLevel = 0;
                strcpy(compMode, "none");
                setCompLevel = true;
                s->comp = true;
                strcpy(progBehaviour, "force compression");
                fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
            }
        } else if(strcmp(argv[i], "-o1") == 0) { /* kompresja 8-bit */
            if(setCompLevel)
                fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
            else {
                s->compLevel = 8;
                strcpy(compMode, "8-bit");
                setCompLevel = true;
                s->comp = true;
                strcpy(progBehaviour, "force compression");
                fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
            }
        } else if(strcmp(argv[i], "-o2") == 0) { /* kompresja 12-bit */
            if(setCompLevel)
                fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
            else {
                s->compLevel = 12;
                strcpy(compMode, "12-bit");
                setCompLevel = true;
                s->comp = true;
                strcpy(progBehaviour, "force compression");
                fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
            }
        } else if(strcmp(argv[i], "-o3") == 0) { /* kompresja 16-bit */
            if(setCompLevel)
                fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], compMode);
            else {
                s->compLevel = 16;
                strcpy(compMode, "16-bit");
                setCompLevel = true;
                s->comp = true;
                strcpy(progBehaviour, "force compression");
                fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], compMode);
            }
        } else if(strcmp(argv[i], "-h") == 0) {
            if(!help_displayed) { /* wyswietlenie pomocy */
                help(stderr);
                help_displayed = true;
            }
        } else if(strcmp(argv[i], "-x") == 0) { /* wymuszenie kompresji */
            if(s->comp || s->decomp)
                fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], progBehaviour);
            else {
                s->comp = true;
                strcpy(progBehaviour, "force compression");
                fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], progBehaviour);
            }
        } else if(strcmp(argv[i], "-d") == 0) { /* wymuszenie dekompresji */
            if(s->comp || s->decomp)
                fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], progBehaviour);
            else {
                s->decomp = true;
                strcpy(progBehaviour, "force decompression");
                fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], progBehaviour);
            }
        } else /* pominiecie niezidentyfikowanych argumentow */
            fprintf(stderr, "%s: %s -> Unknown argument! (ignoring...)\n", argv[0], argv[i]);
}

int fileIsGood(FILE *in, uchar xorCorrectValue, bool displayMsg) {
    uchar c, xor; /* zmienna do odczytania wyniku sumy kontrolnej */
    int i, eof; /* zmienna do sprawdzenia pozycji koncowej */
    const char *impossibleOutput = "Provided file cannot be decompressed since it is not a possible output of this compressor!\n";


    /* Sprawdzenie wielkosci pliku */
    fseek(in, 0, SEEK_END); /* pobranie pozycji koncowej */
    eof = ftell(in);
    fseek(in, 0, SEEK_SET);
    if(eof < 4) {
        fprintf(stderr, "%s", displayMsg ? impossibleOutput : "");
        return 4;
    }

    /* Sprawdzenie poprawnosci oznaczenia na poczatku pliku */
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
    if(displayMsg) /* wyswietlenie wyliczonej sumy kontrolnej na stderr */
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