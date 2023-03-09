#include <stdio.h>
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