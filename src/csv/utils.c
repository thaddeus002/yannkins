/**
 * @file utils.c
 * @brief Utilitaries functions to manipule char strings. 
 */
 
#include "utils.h"
#include <string.h>
#include <stdlib.h>


char *suppress_quotes_new_string(char *string){
    char *result;

    if(string == NULL) { return NULL; }

    result = malloc(sizeof(char) * (strlen(string) + 1));

    if(result == NULL) { return NULL; }

    if ( (string[0]=='"') && (string[strlen(string) - 1]=='"')) {
        strcpy(result, string + 1);
        result[strlen(result) - 1]='\0';
    } else {
        strcpy(result, string);
    }

    return result;
}

void suppress_quotes(char *chaine){
	int l; /* length of the string */
	int i; /* counter */

	l=strlen(chaine);
	if(l>=2){
		if((chaine[0]=='"') && (chaine[l-1]=='"')){
			for(i=0; i<l-2; i++){
				chaine[i]=chaine[i+1];
			}
			chaine[l-2]='\0';
		}
	}
}


void to_upper_case(char *chaine){
	int i; /* counter */

	for(i=0;i<strlen(chaine);i++)
		if((chaine[i]>='a') && (chaine[i]<='z'))
			chaine[i]=chaine[i]+'A'-'a';
}


char *copyString(char *object) {
    char *copy;
    
    if(object == NULL) { return NULL; }
    copy = malloc(sizeof(char) * (strlen(object) + 1));
    strcpy(copy, object);
    return copy;
}

