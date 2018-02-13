/**
 * @file csv.c
 * @brief Utilitaries functions and types to manipulates data. These data
 * can be read from or write in csv files.
 * @author Yannick Garcia
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "utils.h"

/** @brief Buffer size */
#define TAILLE_BUF 10024



/* INTERNAL FUNCTIONS DECLARATIONS */


/**
 * @brief find the index of a column
 * @param table the csv file data
 * @param columnName the name of column to look for
 * @return the index of a column (beginning to zero), or -1 if the column not exist
 */
static int csv_find_column(csv_table_t *table, const char *columnName);


/**
 * @brief Read a line in the given stream.
 *
 * The lines feed or delimiter between doble quotes are not taken in account.
 * @param nbElts the function will put here the number of elements readed, or a negative error code.
 * Must be allocated.
 * @param stream the input stream
 * @param delimiter the column delimiter
 * @return the elements of the line
 */
static char **csv_read_line(int *nbElts, FILE *stream, char delimiter);


/**
 * @brief find if there is a delimiter or a \n in the field
 * @param field string in which look for delimiter
 * @param delimiter the delimiter character
 * @return 0 is neither delimiter or \n was found. 1 otherwise.
 */
static int hasDelimiter(char *field, char delimiter);


/**
 * @brief free the memory occuped by an unique csv_line_t with nbColumns values.
 * @param the pointer to the structur to free
 * @param nbColumns the number of allocated value strings
 */
static void csv_destroy_line(csv_line_t *table, int nbColumns);


/* EXTERNAL FUNCTIONS */


csv_table_t *csv_read_file(char *filename, char delimiter){
    csv_table_t *table; /* the return value */
    FILE *fichier; /* the file to read */
    char **tabElts; /* one line content in a table of strings */
    int nbElts; /* number of element in the line */
    csv_line_t *nouvelleLigne; /* a line to add in table */
    csv_line_t *derniereLigne; /* the last added line */
    int i, j; /* counters */


    if(filename==NULL) return(NULL);

    /* opening file */
    fichier=fopen(filename,"r");
    if(!fichier){
        fprintf(stderr,"Can't open file %s\n", filename);
        return(NULL);
    } else {
        #ifdef DEBUG
        fprintf(stdout,"File %s opened\n", filename);
        #endif
    }

    table=malloc(sizeof(csv_table_t));
    if(table==NULL) return(NULL);
    table->nbCol=0;
    table->nbLig=0;
    table->headers=NULL;
    table->lines=NULL;
    derniereLigne=NULL;

    /* Reading the first line */

    table->headers=csv_read_line(&(table->nbCol), fichier, delimiter);
    if((table->nbCol<=0)||(table->headers==NULL)) {
        csv_destroy_table(table);
        fprintf(stderr,"Fail while reading headers of CSV file %s : code %d\n", filename, table->nbCol);
        return(NULL);
    }

    /* Reading data lines */
    tabElts=NULL;
    j=0; /* number of readed lines */
    do {
        tabElts=csv_read_line(&nbElts, fichier, delimiter);

        if(nbElts==0) {
            /* probably the end of file */
            if (tabElts==NULL) break;
        }

        if(nbElts<=0){
            fprintf(stderr,"Echec de lecture de la ligne %d du fichier CSV %s : code %d\n", j+1, filename, nbElts);
            csv_destroy_table(table);
            return(NULL);
        }

        if((nbElts>0)&&(nbElts!=table->nbCol)){
            fprintf(stderr,"Echec de lecture de la ligne %d du fichier CSV\nNb d'éléments incorrect : %d(!=%d)\n", j+1, nbElts, table->nbCol);
            /* freing memory */
            if(tabElts!=NULL) for(i=0; i<nbElts; i++){
                fprintf(stderr,"%s\n", tabElts[i]);
                if(tabElts[i]!=NULL) free(tabElts[i]);
            }
            free(tabElts);
            // We are not able to correctly read the end of file
            break;
        }

        nouvelleLigne=malloc(sizeof(csv_line_t));
        if(nouvelleLigne==NULL){
            fprintf(stderr,"Echec d'allocation de mémoire\n");
            /* freing memory */
            if(tabElts!=NULL) for(i=0; i<nbElts; i++){
                if(tabElts[i]!=NULL) free(tabElts[i]);
            }
            free(tabElts);
            csv_destroy_table(table);
            return(NULL);
        }

        nouvelleLigne->values=tabElts;
        nouvelleLigne->next=NULL;

        /* adding a new line */
        if(table->lines==NULL) table->lines=nouvelleLigne;
        else derniereLigne->next=nouvelleLigne;
        derniereLigne=nouvelleLigne;
        table->nbLig=table->nbLig + 1;

        // next line
        j++;

    } while (tabElts!=NULL);

    /* end */
    fclose(fichier);
    #ifdef DEBUG
    fprintf(stderr,"Fin de lecture du fichier csv %s\n", filename);
    #endif

    return(table);
}


void csv_destroy_table(csv_table_t *table){
    int i; /* counter */
    csv_line_t *ligne, *suivante; /* look throuth the lines */

    if(table==NULL) return;

    if(table->headers!=NULL){
        for(i=0; i<table->nbCol; i++) if(table->headers[i]!=NULL) free(table->headers[i]);
        free(table->headers);
    }

    ligne=table->lines;
    while(ligne!=NULL){
        if(ligne->values!=NULL){
            for(i=0; i<table->nbCol; i++)
                if(ligne->values[i]!=NULL) free(ligne->values[i]);
            free(ligne->values);
        }
        suivante=ligne->next;
        free(ligne);
        ligne=suivante;
    }

    free(table);
}


csv_table_t *csv_select_lines(csv_table_t *table, const char *columnsName, const char *value){

    return(csv_select_lines_range(table, columnsName, value, value));
}


csv_table_t *csv_select_lines_range(csv_table_t *table, const char *columnsName, const char *min, const char *max){
    csv_table_t *retour; /* return value */
    csv_line_t *ligne; /* a line of the table */
    csv_line_t *nouvelle; /* a line to add at the return value */
    csv_line_t *derniere; /* the last line of retour */
    int i; /* counter */
    int n; /* numero of column */

    if(table==NULL) return(NULL);
    if(columnsName==NULL) return(NULL);
    if(table->nbCol==0) return(NULL);

    n=csv_find_column(table, columnsName);
    if(n<0){
        // column not found
        return(NULL);
    }

    retour=malloc(sizeof(csv_table_t));
    if(retour==NULL) return(NULL);

    retour->nbCol=table->nbCol;
    retour->nbLig=0;
    retour->headers=malloc(retour->nbCol*sizeof(char *));
    if(retour->headers==NULL) return(NULL);
    for(i=0; i<table->nbCol; i++){
        retour->headers[i]=malloc(sizeof(char)*(strlen(table->headers[i])+1));
        if(retour->headers[i]==NULL){
            csv_destroy_table(retour);
            return(NULL);
        }
        strcpy(retour->headers[i], table->headers[i]);
    }
    retour->lines=NULL;
    derniere=NULL;

    ligne=table->lines;

    while(ligne!=NULL){
        if(ligne->values!=NULL) if(ligne->values[n]!=NULL) if((strcmp(ligne->values[n], min)>=0) && (strcmp(ligne->values[n], max)<=0)){
            /* adding the line */

            nouvelle=malloc(sizeof(csv_line_t));
            if(nouvelle==NULL){
                csv_destroy_table(retour);
                return(NULL);
            }

            nouvelle->values=(char **) malloc(table->nbCol * sizeof(char *));
            if(nouvelle->values==NULL){
                csv_destroy_table(retour);
                free(nouvelle);
                return(NULL);
            }

            nouvelle->next=NULL;
            if(derniere==NULL){
                retour->lines=nouvelle;
            } else {
                derniere->next=nouvelle;
            }
            derniere=nouvelle;

            for(i=0; i<table->nbCol; i++){
                if(ligne->values[i]!=NULL){

                    nouvelle->values[i]=NULL;
                    nouvelle->values[i] = (char *) malloc( sizeof(char) * (strlen(ligne->values[i])+1) );
                    if(nouvelle->values[i]==NULL){
                        csv_destroy_table(retour);
                        return(NULL);
                    }
                    strcpy(nouvelle->values[i], ligne->values[i]);
                } else nouvelle->values[i]=NULL;
            }
            (retour->nbLig)++;
        }
        ligne=ligne->next;
    }


    return(retour);
}


int csv_find_value(char value[100], csv_table_t *table, char *columnsName, int line){

    int i, j; /* counters */
    csv_line_t *ligne; /* the line looked for */

    if(table==NULL) return(-1);

    if(columnsName==NULL) return(-2);

    if(line<=0) return(-3);

    if( (table->nbCol==0) || (table->headers==NULL) ) return(-4);

    i=csv_find_column(table, columnsName);
    if(i<0){
        return(-5);
    }

    j=1; /* numero of line */
    ligne=table->lines;

    if(ligne==NULL) return(-6);

    while(j<line){
        ligne=ligne->next;
        if(ligne==NULL) return(-6);
        j++;
    }

    strncpy(value, ligne->values[i], 100-1);
    value[100-1]='\0';

    return(0);
}


csv_table_t *csv_create_table(char **headers, int nbCol){

    csv_table_t *table; /* return value */
    int i; /* counter */

    if((headers==NULL)||(nbCol==0)) return(NULL);

    table = malloc(sizeof(csv_table_t));
    if(table==NULL) return(NULL);

    table->nbCol=nbCol;
    table->nbLig=0;

    table->headers=malloc(nbCol*sizeof(char*));
    if(table->headers==NULL){
        csv_destroy_table(table);
        return(NULL);
    }

    for(i=0; i<nbCol; i++){
        if(headers[i]==NULL) table->headers[i]=NULL;
        else {
            table->headers[i]=malloc((strlen(headers[i])+1)*sizeof(char));

            if(table->headers[i]==NULL){
                csv_destroy_table(table);
                return(NULL);
            }

            strcpy(table->headers[i], headers[i]);
        }
    }

    table->lines=NULL;

    return(table);
}


int csv_add_line(csv_table_t *table, char **content, int contentLength){

    csv_line_t *nouvelle, *derniere; /* lines of the table */
    int i; /* counter */

    if(table==NULL) return(-1);
    if(content==NULL) return(-2);
    if(contentLength>table->nbCol) return(-3);

    derniere=table->lines;
    if(derniere!=NULL) {
        while(derniere->next!=NULL) derniere=derniere->next;
    }

    nouvelle=malloc(sizeof(csv_line_t));
    if(nouvelle==NULL) return(-4);

    nouvelle->next=NULL;
    nouvelle->values=malloc(table->nbCol*sizeof(char *));
    if(nouvelle->values==NULL){
        free(nouvelle);
        return(-5);
    }

    for(i=0; i<table->nbCol; i++){
        if((i<contentLength)&&(content[i]!=NULL)){
            nouvelle->values[i]=malloc((strlen(content[i])+1)*sizeof(char));
            if(nouvelle->values[i]==NULL){
                csv_destroy_line(nouvelle, i);
                return(-6);
            }
            strcpy(nouvelle->values[i], content[i]);

        } else nouvelle->values[i]=NULL;
    }

    if(derniere==NULL) table->lines=nouvelle;
    else derniere->next=nouvelle;
    table->nbLig=table->nbLig+1;
    return(0);
}


void csv_show_table(csv_table_t *table, FILE *output){

    const char vertical='-';
    const char horizontal='|';
    const int largeurCol=10;

    int i,j; /* counters */
    char contenu[largeurCol-2]; /* trunkated content of a cell */
    char format[20]; /* format of strings */
    csv_line_t *ligne; /* going througth the lines */

    if(table==NULL){
        fprintf(stderr, "show_table(): There's no table to show\n");
        return;
    }


    /* superior bordure */
    fprintf(output, "%c", horizontal);
    for(i=0; i<table->nbCol; i++){
        for(j=0; j<largeurCol; j++)
            fprintf(output, "%c", vertical);
        fprintf(output, "%c", horizontal);
    }
    fprintf(output, "\n");

    /* headers */
    fprintf(output, "%c", horizontal);
    for(i=0; i<table->nbCol; i++){
        strncpy(contenu, table->headers[i], largeurCol-3);
        contenu[largeurCol-3]='\0';
        sprintf(format, " %%%ds %%c", largeurCol-2);
        fprintf(output, format, contenu, horizontal);
    }
    fprintf(output, "\n");

    /* bordure 2 */
    fprintf(output, "%c", horizontal);
    for(i=0; i<table->nbCol; i++){
        for(j=0; j<largeurCol; j++)
            fprintf(output, "%c", vertical);
        fprintf(output, "%c", horizontal);
    }
    fprintf(output, "\n");

    /* the lines */
    ligne=table->lines;
    while(ligne!=NULL){
        fprintf(output, "%c", horizontal);
        for(i=0; i<table->nbCol; i++){
            if(ligne->values[i]!=NULL){
                strncpy(contenu, ligne->values[i], largeurCol-3);
                contenu[largeurCol-3]='\0';
            } else contenu[0]='\0';
            sprintf(format, " %%%ds %%c", largeurCol-2);
            fprintf(output, format, contenu, horizontal);
        }
        fprintf(output, "\n");

        ligne=ligne->next;

        /* inferior bordure */
        fprintf(output, "%c", horizontal);
        for(i=0; i<table->nbCol; i++){
            for(j=0; j<largeurCol; j++)
                fprintf(output, "%c", vertical);
            fprintf(output, "%c", horizontal);
        }
        fprintf(output, "\n");
    }
}


int csv_sort_table_decreasing(csv_table_t *table, const char *columnsName){
    int n; /* index of the sorting column */
    int i, j; /* counters */
    csv_line_t **liste, **tri, **tempo; /* tables of pointers to the lines to sort */
    csv_line_t *courant; /* crossing of the linked list */
    char *value1, *value2; /* elements to compare */


    if(table==NULL) return(-1);
    if(columnsName==NULL) return(-2);

    n=csv_find_column(table, columnsName);
    if(n<0) return(-3);

    if(table->nbLig==0) return(-4);

    /* allocating the tables */
    liste=malloc(sizeof(csv_line_t *) * table->nbLig);
    if(liste==NULL) return(-5);

    tri=malloc(sizeof(csv_line_t *) * table->nbLig);
    if(tri==NULL) {
        free(liste);
        return(-6);
    }

    tempo=malloc(sizeof(csv_line_t *) * table->nbLig);
    if(tempo==NULL) {
        free(liste);
        free(tri);
        return(-7);
    }

    /* initializations */
    courant=table->lines;
    for(i = 0; i < table->nbLig; i++){
        liste[i]=courant;
        tri[i]=NULL;
        if(courant!=NULL) courant=courant->next;
    }

    /* sorting */
    tri[0]=liste[0];

    for(i=1; i<table->nbLig; i++){

        /* looking for the element position */
        for(j=0; j<i; j++){
            /* comparaison */
            value1=liste[i]->values[n];
            value2=tri[j]->values[n];
            if(strcmp(value1, value2)>0) break;
        }

        /* prospective shift in the bottom of the table */
        if(j<i){
            memcpy(tempo, tri+j, sizeof(csv_line_t *)*(table->nbLig-j-1));
            memcpy(tri+j+1, tempo, sizeof(csv_line_t *)*(table->nbLig-j-1));
        }

        /* placement of the element */
        tri[j]=liste[i];
    }

    /* correcting the linkage of the list */
    table->lines=tri[0];
    for(i = 0; i < table->nbLig - 1; i++){
        tri[i]->next=tri[i+1];
    }
    tri[table->nbLig - 1]->next=NULL;

    /* freeing memory and end */
    free(liste);
    free(tri);
    free(tempo);
    return(0);
}


int csv_merge_tables(csv_table_t *table1, csv_table_t *table2){

    int i; /* counter */
    csv_line_t *derniereLigne; /* the last line of table1 */

    /* verification of arguments */
    if(table1==NULL) return(-1);
    if(table2==NULL) return(0);

    /* verification of columns' headers */
    if(table1->nbCol!=table2->nbCol) return(-1);

    for(i=0; i<table1->nbCol; i++){
        if(strcmp(table1->headers[i], table2->headers[i])) return(-2);
    }


    /* merge */
    derniereLigne=table1->lines;
    if(derniereLigne!=NULL)
        while(derniereLigne->next!=NULL) derniereLigne=derniereLigne->next;
    if(derniereLigne==NULL)
        table1->lines=table2->lines;
    else
        derniereLigne->next=table2->lines;

    /* end */
    return(0);
}


csv_table_t *csv_select_columns(csv_table_t *table, char **searchedElts, int nbSearchedElts, int *nbFoundElts){

    int i=0; // counting columns
    int j; //counter
    char *entete;
    csv_table_t *selection;
    int *colonnes; // selected columns
    char **entetes_trouves;
    csv_line_t *ligne; // crossing the lines

    *nbFoundElts=0;

    if(table->headers==NULL) {
        return NULL;
    }

    entetes_trouves=malloc(sizeof(char *)*nbSearchedElts);
    colonnes=malloc(sizeof(int)*nbSearchedElts);

    entete=table->headers[i];

    while ((entete!=NULL)&&(i<table->nbCol)){

        int trouve = 0;

        for(j=0; j<nbSearchedElts; j++){
            if(!strcmp(entete, searchedElts[j])) {
                trouve = 1;
                break;
            }
        }

        if(trouve) {
            colonnes[*nbFoundElts]=i;
            entetes_trouves[*nbFoundElts]=entete;
            (*nbFoundElts)++;
        }

        if(*nbFoundElts==nbSearchedElts){
            break;
        }

        i++;
        if(i<table->nbCol){
            entete=table->headers[i];
        } else {
            entete=NULL;
        }
    }


    selection = csv_create_table(entetes_trouves, *nbFoundElts);

    ligne = table->lines;

    while (ligne!=NULL){

        char **contenu = malloc (sizeof(char*) * *nbFoundElts);

        for(j=0; j<*nbFoundElts; j++){

            char *value = ligne->values[colonnes[j]];
            contenu[j]=malloc(sizeof(char)*(strlen(value)+1));
            strcpy(contenu[j], value);
        }

        csv_add_line(selection, contenu, *nbFoundElts);

        ligne=ligne->next;
    }


    free(entetes_trouves);
    free(colonnes);
    return selection;
}


int csv_write_file(char *filename, csv_table_t *table, char delimiter){
    int i, j; /* counters */
    csv_line_t *courant; /* crossing the lines */
    FILE *fo; /* file descriptor */

    if(filename==NULL) {
        fo=stdout;
    } else {
        fo=fopen(filename, "w");
    }
    if(fo==NULL){
        fprintf(stderr, "Can't open file %s", filename);
        return(-1);
    }


    /* headers */
    for(j=0; j<table->nbCol; j++) {
        if(hasDelimiter(table->headers[j], delimiter)) {
            fprintf(fo, "\"%s\"", table->headers[j]);
        } else {
            fprintf(fo, "%s", table->headers[j]);
        }

        if(j<table->nbCol-1) {
            fprintf(fo, "%c", delimiter);
        } else {
            fprintf(fo, "\n");
        }
    }

    /* lines*/

    courant = table->lines;
    for(i=0; i<table->nbLig; i++) {
        for(j=0; j<table->nbCol; j++) {

            if(courant->values[j]!=NULL) {
                if(hasDelimiter(courant->values[j], delimiter)){
                    fprintf(fo, "\"%s\"", courant->values[j]);
                } else {
                    fprintf(fo, "%s", courant->values[j]);
                }
            }
            if(j<table->nbCol-1) {
                fprintf(fo, "%c", delimiter);
            } else {
                fprintf(fo, "\n");
            }
        }

        courant=courant->next;
    }


    if(filename!=NULL) fclose(fo);
    return(0);
}


int csv_truncate_column(csv_table_t *table, char *columnsName, int ltk){

    int n; // number of column
    int trouve; // column found?
    int i; // counter
    csv_line_t *ligne; // crossing lines

    trouve = 0;

    for(i=0; i<table->nbCol; i++){
        if(!strcmp(columnsName, table->headers[i])){
            n=i;
            trouve=1;
            break;
        }
    }

    if(!trouve){
        return 1;
    }

    ligne = table->lines;
    while(ligne!=NULL){
        if(strlen(ligne->values[n]) > ltk){
            ligne->values[n][ltk]='\0';
        }

        ligne=ligne->next;
    }

    return 0;
}




/************************************************************************/
/*                INTERNAL FUNCTIONS IMPLEMENATATION                    */
/************************************************************************/


static int csv_find_column(csv_table_t *table, const char *columnName){

    int n; /**< column number */
    char entete[100]; /**< a column header */
    char nomColMaj[100]; /**< the looking for column name in uercase */

    if(table==NULL) return(-1);

    if(columnName==NULL) return(-2);

    if( (table->nbCol==0) || (table->headers==NULL) ) return(-3);

    strncpy(nomColMaj, columnName, 100-1);
    nomColMaj[100-1]='\0';
    to_upper_case(nomColMaj);

    n=0;
    while(n<table->nbCol){
        strncpy(entete, table->headers[n], 100-1);
        entete[100-1]='\0';
        to_upper_case(entete);
        if(!strcmp(entete, nomColMaj)) break;
        n++;
    }

    if(n==table->nbCol) return(-1);
    return(n);
}


static char **csv_read_line(int *nbElts, FILE *stream, char delimiter){

    char **retour; /* return value */
    int nA; /* number of allocations of X char* */
    char ligne[TAILLE_BUF]; /* a line of the file */
    int i,k,l,m; /*counters */
    char **tempo; /* intermediate table of strings */
    int finLigne; /* indicate the end of the reading line */
    char elt[100]; /* an element read on the line */
    int guillemetsOuverts; /* indicate the opening of quotes */
    char *err; /* return of fgets() */


    *nbElts=0;
    nA=1;
    retour=malloc(nA*100*sizeof(char*));
    if(retour==NULL){
        *nbElts=-1;
        return(NULL);
    }

    /* read the line */
    i=0; /* index of found element */
    m=0; /* index of position in the string "elt" */
    finLigne=0; elt[0]='\0'; guillemetsOuverts=0;

    while(!finLigne){

        err=fgets(ligne, TAILLE_BUF, stream);

        if(err==NULL) { /* End of file reached ? */
            if(i==0) {
                free(retour);
                return(NULL);
            }
            return(retour);
        }

        k=0; /* actual position on the line */

        while( (k<TAILLE_BUF) && (ligne[k]!='\0') ){

            if( (!guillemetsOuverts) && (ligne[k]=='\n') ) {
                break;
            }

            /* If found delimiter add the element */
            if( (ligne[k]==delimiter) && (!guillemetsOuverts) ){
                elt[m]='\0';
                /* may be quotes around the field */
                suppress_quotes(elt);
                retour[i]=malloc((strlen(elt)+1)*sizeof(char));
                if(retour[i]==NULL){
                    *nbElts=-2;
                    return(retour);
                }
                strcpy(retour[i], elt);
                i++;
                (*nbElts)++;
                /* re-initialization */
                elt[0]='\0'; m=0;

                /* allocation of additional memory if necessary */
                if(i==nA*100){
                    nA++;
                    tempo=malloc(nA*100*sizeof(char*));
                    if(tempo==NULL){
                        *nbElts=-3;
                        return(retour);
                    }
                    for(l=0; l<i; l++) tempo[l]=retour[l];
                    free(retour);
                    retour=tempo;
                }

            /* Otherwise add the character at element name */
            } else {
                if((m<100-1)&&(ligne[k]!='\r')){
                    elt[m]=ligne[k];
                    m++;
                }
                if(ligne[k]=='"') {
                    guillemetsOuverts=guillemetsOuverts?0:1;
                }
            }

            /* next character */
            k++;

        } // end of loop : while not end of string or of buffer

        /* If we have read all the buffer, we must load another */
        if(k==TAILLE_BUF) continue;

        // if the field is not entirely read, we must load another buffer
        if(guillemetsOuverts) continue;

        if((ligne[k]=='\0')||(ligne[k]=='\n')) {
            /* add the last element */
            elt[m]='\0';
            /* may be quotes around field */
            suppress_quotes(elt);
            retour[i]=malloc((strlen(elt)+1)*sizeof(char));
            if(retour[i]==NULL){
                *nbElts=-2;
                return(retour);
            }
            strcpy(retour[i], elt);
            (*nbElts)++;

            /* End of line detected */
            #ifdef DEBUG
            fprintf(stdout, "%d found elements\n", *nbElts);
            fprintf(stdout, "last element : %s - %s\n", retour[i], elt);
            #endif
            finLigne=1;
        }
    } // end of line

    return(retour);
}


static int hasDelimiter(char *field, char delimiter) {

    char *current = field;

    while( *current != '\0' ) {

        if( (*current == delimiter) || (*current == '\n') ) {
            return 1;
        }

        current++;
    }

    return 0;
}


static void csv_destroy_line(csv_line_t *line, int nbColumns){

    int i; // counter

    if(line == NULL) return;

    if(line->values!=NULL){
        for(i=0; i<nbColumns; i++)
            if(line->values[i]!=NULL) free(line->values[i]);
        free(line->values);
    }
    free(line);
}
