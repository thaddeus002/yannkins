/**
 * \file log_analyse.c
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csv/csv.h"
#include "log_analyse.h"



/**
 * Get an unique integer for a given month.
 * \param date the date in format "YYYY-MM-DD ..."
 */
static int get_num_month(char *date) {

    char year[5];
    char month[3];
    int iyear, imonth;

    strncpy(year, date, 4); year[4]='\0';
    strncpy(month, date+5, 2); month[2]='\0';

    iyear = atoi(year);
    imonth = atoi(month);

    return (iyear*100 + imonth);
}



/**
 * Get the id of the previous month of a given one.
 * \param the id af a month
 * \return the id of the month before
 */
static int decrease_month(int month) {

    int iyear, imonth;

    iyear = month / 100;
    imonth = month % 100;

    imonth--;
    if(imonth==0) {
        imonth = 12;
        iyear--;
    }

    return (iyear*100 + imonth);
}


/**
 * Add a new line to the result table.
 */
static void add_line(table_csv_t *result, int numMonth, int number){

    char sMonth[100];
    char sNumber[100];
    char *content[2];

    sprintf(sMonth, "%d", numMonth);
    sprintf(sNumber, "%d", number);
    content[0]=sMonth;
    content[1]=sNumber;

    csv_add_line(result, content, 2);
}


/**
 * Add a new line to the result table. *
 */
static void add_line_authors(table_csv_t *result, int numMonth, int *numbers){

    char **content;
    int i;

    content=malloc(result->nbCol * sizeof(char *));

    for(i=0; i<result->nbCol; i++) {
        content[i] = malloc(20 * sizeof(char));
    }


    sprintf(content[0], "%d", numMonth);

    for(i=1; i<result->nbCol; i++) {
       sprintf(content[i], "%d", numbers[i-1]);
    }


    csv_add_line(result, content, result->nbCol);


    for(i=0; i<result->nbCol; i++) {
        free(content[i]);
    }
    free(content);
}




table_csv_t *nb_by_month(table_csv_t *table, char *date_header) {

    table_csv_t *result;
    char *headers[2] = { "month", "number of commits" };
    int i; // line counter
    int numMonth; // month's number
    int number; // counter of occurences

    csv_sort_table_decreasing(table, date_header);

    result = csv_create_table(headers, 2);

    number = 0;
    numMonth = 0;

    for(i=1; i<=table->nbLig; i++) {

        int err;
        char value[100];
        int month;

        err = csv_find_value(value, table, date_header, i);

        if(err) {
            fprintf(stderr, "An error occured line %d: %d\n", i, err);
            continue;
        }

        month = get_num_month(value);
        if(numMonth==0) { numMonth = month; }

        if(month == numMonth) {
            number++;
        } else {

            while(numMonth != month) {

                add_line(result, numMonth, number);

                // reinit
                numMonth = decrease_month(numMonth);
                number = 0;
            }

            number = 1;
        }
    }

    return result;
}



/**
 * Is the list containing this element?
 */
static int contains(char **list, char *element) {

    int i = 0;

    while(list[i] != NULL) {

        if (!strcmp(list[i], element)) {
            return 1;
        }
        i++;
    }

    return 0;
}



/**
 * Get a list of differents authors
 * \param
 * \param
 * \param
 * \return a newly allocated table ending with NULL
 */
static char **get_authors(table_csv_t *table, int *nbAuthors, char *author_header) {

    int size = 20;
    int nb = 0;
    char **authors;
    ligne_csv_t *line;
    int col = -1;
    int i;

    // find column number
    for(i = 0; i<table->nbCol; i++) {
        if(!strcmp(author_header, table->entetes[i])) {
            col = i;
        }
    }

    if(col < 0) {
        fprintf(stderr, "Error : column \"%s\" not found\n", author_header);
        return NULL;
    }

    // search in table
    authors = malloc(size * sizeof(char *));
    authors[0] = NULL;


    line = table->lignes;
    while(line != NULL) {

        char *author = line->valeurs[col];

        if(!contains(authors, author)){

            if(nb == size-1) {
                size += 20;
                authors = realloc(authors, (size) * sizeof(char *));
            }

            authors[nb] = malloc(strlen(author+1));
            strcpy(authors[nb], author);
            authors[nb+1] = NULL;
            nb++;
        }

        line = line->next;
    }

    *nbAuthors = nb;
    if(nb == 0) {
        free(authors);
        authors = NULL;
    }
    return authors;
}






static void free_authors(char **authors) {

    int i = 0;

    while(authors[i]!=NULL){
        free(authors[i]);
        i++;
    }

    free(authors);
}



int get_authors_number(table_csv_t *vcsLogTable) {

    int nbAuthors;
    char **authors = get_authors(vcsLogTable, &nbAuthors, "author");

    free_authors(authors);

    return nbAuthors;
}



static void add_count(int *numbers, char **authors, char *author){

    int pos = -1;
    int i = 0;

    // find column number
    while(authors[i] != NULL) {
        if(!strcmp(author, authors[i])) {
            pos = i;
        }
        i++;
    }

    if(pos < 0) return;

    numbers[pos]++;
    numbers[i]++;
}



table_csv_t *nb_by_month_and_by_authors(table_csv_t *table, char *date_header, char *author_header) {

    table_csv_t *result;
    char **headers;
    char **authors;
    int nbAuthors = 0;
    int i; // line counter
    int j; // counter
    int numMonth; // month's number
    int *numbers; // counter of occurences

    csv_sort_table_decreasing(table, date_header);

    authors = get_authors(table, &nbAuthors, author_header);

    if(nbAuthors == 0) { return NULL; }

    headers = malloc((nbAuthors + 2)*sizeof(char *));

    headers[0]="month";
    headers[nbAuthors+1]="total";

    for(j=1; j<=nbAuthors; j++) {
        headers[j]=authors[j-1];
    }

    result = csv_create_table(headers, nbAuthors+2);

    // last column is for total
    numbers = malloc((nbAuthors+1) * sizeof(int));
    for(j=0; j<nbAuthors+1; j++) numbers[j] = 0;

    numMonth = 0;

    for(i=1; i<=table->nbLig; i++) {

        int err;
        char value[100];
        char author[100];
        int month;

        err = csv_find_value(value, table, date_header, i);

        if(err) {
            fprintf(stderr, "An error occured line %d: %d\n", i, err);
            continue;
        }

        err = csv_find_value(author, table, author_header, i);

        if(err) {
            fprintf(stderr, "An error occured line %d: %d\n", i, err);
            continue;
        }


        month = get_num_month(value);
        if(numMonth==0) { numMonth = month; }

        while(numMonth != month) {

            add_line_authors(result, numMonth, numbers);

            // reinit
            numMonth = decrease_month(numMonth);
            for(j=0; j<nbAuthors+1; j++) numbers[j] = 0;
        }

        add_count(numbers, authors, author);
    }

    free_authors(authors);
    free(headers);
    return result;
}


