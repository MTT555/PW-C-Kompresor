#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define BUF_SIZE 256

int main(int argc, char*argv[]){

	//Wczytuje tekst uzytkownika z pliku
	//Nazwe pliku podajemy jako pierwszy argument wywolania
	
	FILE *in = argc > 1 ? fopen(argv[1], "r"):stdin;
	
	char *temp = malloc(sizeof(char)*BUF_SIZE); // bufor tymczasowy przechowujacy kazdy odczytywany wyraz
	
	int count['z'-'a' + 1]; //tablica liczaca wystapienia poszczegolnych wyrazow

	for (int i = 0; i < 'z'-'a'+1; i++)count[i]=0;
	//przy czym zerowemu indeksowi tablicy odpowiada pierwszy znak, drugiemu drugi itp.
	while (fscanf(in, "%s", temp) != EOF){
	//iterujemy po wczytanym ciagu znakow i liczymy wystapienia kazdego znaku
        while (temp != NULL && *temp != '\0') {
            if(*temp >= 'a' && *temp <= 'z') count[*temp - 'a']++; 
            temp++;
        }	
	}

#ifdef DEBUG

	//wypisujemy tablice wystapien
	
	for (int i = 0; i < 'z'-'a'+1; i++) printf("Znak: %c, wystapien: %d\n",i+'a', count[i]);

#endif

return 0;
}
