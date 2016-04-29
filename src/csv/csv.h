/**
 * @file csv.h
 * @brief Utilitaries functions and types to manipulates data. These data
 * can be read from or write in csv files. 
 * @author Yannick Garcia
 */

#ifndef CSV_H_
#define CSV_H_

#include <stdio.h>


/** Structur representing one csv line or one serie of data */
typedef struct ligne_csv_t_ {
	char **valeurs; /**< fields values */

	struct ligne_csv_t_ *next; /**< to make a linked list of csv lines */
} ligne_csv_t;


/** An entire csv file */
typedef struct{
	int nbCol; /**< number of colums - Don't directly modify this value */
	int nbLig; /**< number of lines - Don't directly modify this value */
	char **entetes; /**< columns' names */

	ligne_csv_t *lignes; /**< a linked list of lines */
} table_csv_t;



/************************************************************************/
/*                            THE FUNCTIONS                             */
/************************************************************************/


/**
 * @brief read a csv file.
 *
 * The quotes between fields are removed.
 * @param nomFichier the name of csv file
 * @param separateur the split character
 * @return NULL if the file can not be read
 */
table_csv_t *lecture_fichier_csv_entier(char *nomFichier, char separateur);


/**
 * @brief Create a new table with less columns.
 * @param table the origin table
 * @param elementsCherches table of columns names to look for
 * @param nbElementsCherches number of elements in elementsCherches
 * @param nbElementsTrouves the function will plce here  the number of columns found 
 * @return the new table
 */
table_csv_t *selectionne_colonnes(table_csv_t *table, char **elementsCherches, int nbElementsCherches, int *nbElementsTrouves);


/**
 * @brief Tronque si nécessaire la longueur des champs d'une colonne.
 * @param table la table à modifier
 * @param nom_colonne le nom de la colonne à traiter
 * @param longueur la longueur maximale à conserver
 * @return 0 en cas de réussite
 */
int tronquer_colonne(table_csv_t *table, char *nom_colonne, int longueur);


/**
 * @brief free memory occuped by a table_csv_t
 * @param table the struct to free
 */
void destroy_table_csv(table_csv_t *table);


/**
 * @brief select the lines with property name=value.
 * @param table the table where look for data
 * @param nomColonne name of column
 * @param valeur value to find in the column
 * @return the new table with only the selected lines
 */
table_csv_t *selectionne_lignes(table_csv_t *table, const char *nomColonne, const char *valeur);


/**
 * @brief select the lines with property min<=value<=max.
 * @param table the table where look for data
 * @param nomColonne name of column
 * @param min the minimun value for the column
 * @param max the maximun value for the column
 * @return the new table with only the selected lines
 */
table_csv_t *selectionne_lignes_plage(table_csv_t *table, const char *nomColonne, const char *min, const char *max);


/**
 * @brief find the value at a given position.
 * @param valeur the function will put here the result - must be allocated
 * @param table the data
 * @param nomColonne column name to look for
 * @param numLigne line number (beginning at 1) to look for
 * @return a non null code if un error occured
 */
int cherche_valeur(char valeur[100], table_csv_t *table, char *nomColonne, int numLigne); 


/**
 * @brief create an empty data table
 * @param entetes headers of columns
 * @param nbCol number of columns 
 * @return the newly created table
 */
table_csv_t *cree_table(char **entetes, int nbCol);


/**
 * @brief Add a line to csv table
 *
 * If the line to add has less elements than the number of columns, the line
 * will be complete with NULL. If there are more element an error code
 * will be reurned.
 * @param table the csv table
 * @param contenu the data to add
 * @param nbContenu to number of elements in contenu
 * @return a non null code if un error occured
 */
int ajoute_ligne(table_csv_t *table, char **contenu, int nbContenu);


/**
 * @brief Show the table.
 * @param table the data to print
 * @param flux the stream where print the data
 */
void affiche_table(table_csv_t *table, FILE *flux);


/**
 * @brief sort the table in decreasing order for a given column
 * @param table the table to sort
 * @param nomColonne the name of the column to sort by
 * @return a non null code if un error occured
 */
int tri_table_decroissant(table_csv_t *table, const char *nomColonne);


/**
 * @brief merge two tables.
 *
 * The first table will be added the lines of the seconds if the headers lines are
 * identicals.
 * @param table1 the first table 
 * @param table2 the second table
 * @return a non null code if un error occured
 */
int fusionne_tables(table_csv_t *table1, table_csv_t *table2);


/**
 * @brief write a file with the content of a table_csv_t
 *
 * In case of \r\n or delimiter character in a field, this field will be between
 * doble quotes.
 * @param nomFichier the name of the file to create
 * @param table the data
 * @param separateur fields delimiter
 * @return a non null code if un error occured
 */
int ecrit_csv(char *nomFichier, table_csv_t *table, char separateur);


#endif
