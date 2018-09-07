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

void suppress_quotes(char *string){
    int l; /* length of the string */
    int i; /* counter */

    l=strlen(string);
    if(l>=2){
        if((string[0]=='"') && (string[l-1]=='"')){
            for(i=0; i<l-2; i++){
                string[i]=string[i+1];
            }
            string[l-2]='\0';
        }
    }
}


void to_upper_case(char *string){
    int i; /* counter */

    for(i=0;i<strlen(string);i++)
        if((string[i]>='a') && (string[i]<='z'))
            string[i]=string[i]+'A'-'a';
}

