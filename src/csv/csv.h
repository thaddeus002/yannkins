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


/* remplie le champ valeur avec le contenu de la colonne demandée pour le numéro de ligne voulu */
/* renvoie un code d'erreur non nul en cas d'erreur */
int cherche_valeur(char valeur[100], table_csv_t *table, char *nomColonne, int numLigne); 


/* créé une nouvelle table vide avec les entetes donnés */
table_csv_t *cree_table(char **entetes, int nbCol);


/* ajoute une ligne de données à la table */
/* Si il y a moins d'éléments que de colonnes dans la table, les autres éléments sont initialisés à NULL */
/* Si il y en a plus, renvoie un code d'erreur non nul */
int ajoute_ligne(table_csv_t *table, char **contenu, int nbContenu);


/* affiche la table dans le flux passé en argument */
void affiche_table(table_csv_t *table, FILE *flux);


/* tri la table par ordre decroissant pour la colonne souhaitée */
/* renvoie un code non nul en cas d'échec */
int tri_table_decroissant(table_csv_t *table, const char *nomColonne);


/* fusion de deux tables */
/* la table1 se verra ajouter les lignes de la table deux si les lignes d'entete sont identiques */
/* Renvoie 0 en cas de réussite */ 
int fusionne_tables(table_csv_t *table1, table_csv_t *table2);


/* A COMPLETER */
/* créée un fichier avec le contenu de la table */
/* en cas de \r\n ou de separateur dans un champ, passage du champ entre guillemets */
int ecrit_csv(char *nomFichier, table_csv_t *table, char separateur);


#endif
