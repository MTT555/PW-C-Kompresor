#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "countCharacters.h"
#define BUF_SIZE 256

int main(int argc, char*argv[]){

	//Wczytuje tekst uzytkownika z pliku
	//Nazwe pliku podajemy jako pierwszy argument wywolania
	
	FILE *in = argc > 1 ? fopen(argv[1], "r"):stdin;
	
	count *head; //tworze glowe listy w ktorej bede przechowywal zliczenia
	runCounter(&head);

	char c;
	//wczytuje i zliczam znak po znaku
	while((c=fgetc(in)) != EOF){

	if(checkIfElementIsOnTheList(&head, c) == 1){
	addToTheList(&head, c);
	}

	}
	fclose(in);

#ifdef DEBUG

	//wypisujemy liste z wystapieniami
	
	showList(&head);
#endif

return 0;
}
