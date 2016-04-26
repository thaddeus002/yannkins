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
 * Fct de lecture d'un fichier CSV.
 * Enleve les guillemets autour des champs.
 * @return NULL si le fichier ne peut être ouvert
 */
table_csv_t *lecture_fichier_csv_entier(char *nomFichier, char separateur);

/* Fct de lecture d'un fichier CSV */
/* Séparateur de champs : ";" */
/* ATTENTION : cette fct est obsolete - doit disparaitre */
ligne_csv_t *lecture_fichier_csv(char *nomFichier, char **elementsCherches, int nbElementsCherches);


/**
 * @brief Créée une nouvelle table avec un nb de colonnes réduit.
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




/* libération de la mémoire occupée par un tableau de lignes csv */
/* ATTENTION : cette fct doit sortir de l'API */
void detruit_ligne_csv(ligne_csv_t *table, int nbColonnes);

/* libération de la mémoire occupée par une table csv */
void destroy_table_csv(table_csv_t *table);


/* renvoie une table avec les lignes correspondant à nomColonne==valeur */
table_csv_t *selectionne_lignes(table_csv_t *table, const char *nomColonne, const char *valeur);
/* renvoie une table avec les lignes correspondant à min<=valeur(nomColonne)<=max */
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
/* écrit dans le flux au format BUFS ASCII */
int ecrit_csv_BUFS(FILE *flux, table_csv_t *table, char separateur);


#endif
