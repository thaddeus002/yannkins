/**
 * @file csv.c
 * @brief Utilitaries functions and types to manipulates data. These data
 * can be read from or write in csv files with ';' delimiter. 
 * @author Yannick Garcia
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"

/** @brief Buffer size */
#define TAILLE_BUF 10024



/* INTERNAL FUNCTIONS DECLARATIONS */

/**
 * @brief Delete the doble quotes of a string.
 * @param chaine the string to modify
 */
static void supprime_guillemets(char *chaine);


/**
 * @brief Convert à String to uppercase.
 * @param chaine the string to modify
 */
static void passe_en_majuscules(char *chaine);


/**
 * @brief find the index of a column
 * @param table the csv file data
 * @param nomColonne the name of column to look for
 * @return the index of a column (beginning to zero), or -1 if the column not exist
 */
static int cherche_colonne(table_csv_t *table, const char *nomColonne);


/**
 * @brief Read a line in the given file.
 *
 * The lines feed or delimiter between doble quotes are not taken in account.
 * @param nbElts the function will put here the number of element readed, or a negative error code.
 * Must be allocated.
 * @param fichier the input stream
 * @param separateur the column delimiter
 * @return the elements of the line
 */
static char **lit_ligne(int *nbElts, FILE *fichier, char separateur);


/**
 * @brief find if there is a delimiter or a \n in the field
 * @param field string in which look for delimiter
 * @param delimiter the delimiter character
 * @return 0 is neither delimiter or \n was found. 1 otherwise.
 */
static int hasDelimiter(char *field, char delimiter);


/**
 * @brief free the memory occuped by a ligne_csv_t.
 * @param the pointer to the structur to free
 * @param nbColonnes the number of allocated value strings
 */
static void detruit_ligne_csv(ligne_csv_t *table, int nbColonnes);


/* EXTERNAL FUNCTIONS */


table_csv_t *lecture_fichier_csv_entier(char *nomFichier, char separateur){
	table_csv_t *table; /* the return value */
	FILE *fichier; /* the file to read */
	char **tabElts; /* one line content in a table of strings */
	int nbElts; /* number of element in the line */
	ligne_csv_t *nouvelleLigne; /* a line to add in table */
	ligne_csv_t *derniereLigne; /* the last added line */
	int i, j; /* counters */


	if(nomFichier==NULL) return(NULL);

	/* opening file */
	fichier=fopen(nomFichier,"r");
	if(!fichier){
		fprintf(stderr,"Can't open file %s\n", nomFichier);
		return(NULL);
	} else {
		#ifdef DEBUG
		fprintf(stdout,"File %s opened\n", nomFichier);
		#endif
	}

	table=malloc(sizeof(table_csv_t));
	if(table==NULL) return(NULL);
	table->nbCol=0;
	table->nbLig=0;
	table->entetes=NULL;
	table->lignes=NULL;
	derniereLigne=NULL;

	/* Reading the first line */

	table->entetes=lit_ligne(&(table->nbCol), fichier, separateur);
	if((table->nbCol<=0)||(table->entetes==NULL)) {
		destroy_table_csv(table);
		fprintf(stderr,"Fail while reading headers of CSV file %s : code %d\n", nomFichier, table->nbCol);
		return(NULL);
	}
	
	/* Reading data lines */
	tabElts=NULL;
	j=0; /* number of readed lines */ 
	do {
		tabElts=lit_ligne(&nbElts, fichier, separateur);

		if(nbElts==0) {
			/* probably the end of file */
			if (tabElts==NULL) break;
		}

		if(nbElts<=0){
			fprintf(stderr,"Echec de lecture de la ligne %d du fichier CSV %s : code %d\n", j+1, nomFichier, nbElts);
			destroy_table_csv(table);
			return(NULL);
		}

		if((nbElts>0)&&(nbElts!=table->nbCol)){
			fprintf(stderr,"Echec de lecture de la ligne %d du fichier CSV\nNb d'éléments incorrect : %d(!=%d)\n", j+1, nbElts, table->nbCol);
			/* freing memory */
			if(tabElts!=NULL) for(i=0; i<nbElts; i++){
				if(tabElts[i]!=NULL) free(tabElts[i]);
			}
			free(tabElts);
			destroy_table_csv(table);
			/* stop */
			return(NULL);
		}

		nouvelleLigne=malloc(sizeof(ligne_csv_t));
		if(nouvelleLigne==NULL){
			fprintf(stderr,"Echec d'allocation de mémoire\n");
			destroy_table_csv(table);
			return(NULL);
		}

		nouvelleLigne->valeurs=tabElts;
		nouvelleLigne->next=NULL;

		/* adding a new line */
		if(table->lignes==NULL) table->lignes=nouvelleLigne;
		else derniereLigne->next=nouvelleLigne;
		derniereLigne=nouvelleLigne;
		j++;
		table->nbLig=table->nbLig + 1;

	} while (tabElts!=NULL);

	/* end */
	fclose(fichier);
	#ifdef DEBUG
	fprintf(stderr,"Fin de lecture du fichier csv %s\n", nomFichier);
	#endif

	return(table);
}


void destroy_table_csv(table_csv_t *table){
	int i; /* counter */
	ligne_csv_t *ligne, *suivante; /* parcourt des lignes */

	if(table==NULL) return;

	if(table->entetes!=NULL){
		for(i=0; i<table->nbCol; i++) if(table->entetes[i]!=NULL) free(table->entetes[i]);
		free(table->entetes);
	}
	return;
	ligne=table->lignes;
	while(ligne!=NULL){
		if(ligne->valeurs!=NULL){
			for(i=0; i<table->nbCol; i++)
				if(ligne->valeurs[i]!=NULL) free(ligne->valeurs[i]);
			free(ligne->valeurs);
		}
		suivante=ligne->next;
		free(ligne);
		ligne=suivante;
	}

	free(table);
}



table_csv_t *selectionne_lignes(table_csv_t *table, const char *nomColonne, const char *valeur){

	return(selectionne_lignes_plage(table, nomColonne, valeur, valeur));
}


table_csv_t *selectionne_lignes_plage(table_csv_t *table, const char *nomColonne, const char *min, const char *max){
	table_csv_t *retour; /* return value */
	ligne_csv_t *ligne; /* a line of the table */
	ligne_csv_t *nouvelle; /* a line to add at the return value */
	ligne_csv_t *derniere; /* the last line of retour */
	int i; /* counter */
	int n; /* numero of column */

	if(table==NULL) return(NULL);
	if(nomColonne==NULL) return(NULL);
	if(table->nbCol==0) return(NULL);

	n=cherche_colonne(table, nomColonne);
	if(n<0){
		// column not found
		return(NULL);
	}

	retour=malloc(sizeof(table_csv_t));
	if(retour==NULL) return(NULL);

	retour->nbCol=table->nbCol;
	retour->nbLig=0;
	retour->entetes=malloc(retour->nbCol*sizeof(char *));
	if(retour->entetes==NULL) return(NULL);
	for(i=0; i<table->nbCol; i++){
		retour->entetes[i]=malloc(sizeof(char)*(strlen(table->entetes[i])+1));
		if(retour->entetes[i]==NULL){
			destroy_table_csv(retour);
			return(NULL);
		}
		strcpy(retour->entetes[i], table->entetes[i]);
	}
	retour->lignes=NULL;
	derniere=NULL;

	ligne=table->lignes;

	while(ligne!=NULL){
		if(ligne->valeurs!=NULL) if(ligne->valeurs[n]!=NULL) if((strcmp(ligne->valeurs[n], min)>=0) && (strcmp(ligne->valeurs[n], max)<=0)){
			/* adding the line */

			nouvelle=malloc(sizeof(ligne_csv_t));
			if(nouvelle==NULL){
				destroy_table_csv(retour);
				return(NULL);
			}

			nouvelle->valeurs=(char **) malloc(table->nbCol * sizeof(char *));
			if(nouvelle->valeurs==NULL){
				destroy_table_csv(retour);
				free(nouvelle);
				return(NULL);
			}

			nouvelle->next=NULL;
			if(derniere==NULL){
				retour->lignes=nouvelle;
			} else {
				derniere->next=nouvelle;
			}
			derniere=nouvelle;

			for(i=0; i<table->nbCol; i++){
				if(ligne->valeurs[i]!=NULL){

					nouvelle->valeurs[i]=NULL;
					nouvelle->valeurs[i] = (char *) malloc( sizeof(char) * (strlen(ligne->valeurs[i])+1) );
					if(nouvelle->valeurs[i]==NULL){
						destroy_table_csv(retour);
						return(NULL);
					}
					strcpy(nouvelle->valeurs[i], ligne->valeurs[i]);
				} else nouvelle->valeurs[i]=NULL;
			}
			(retour->nbLig)++;
		}
		ligne=ligne->next;
	}


	return(retour);
}


int cherche_valeur(char valeur[100], table_csv_t *table, char *nomColonne, int numLigne){

	int i, j; /* compteurs */
	ligne_csv_t *ligne; /* la ligne demandée */

	if(table==NULL) return(-1);

	if(nomColonne==NULL) return(-2);

	if(numLigne<=0) return(-3);

	if( (table->nbCol==0) || (table->entetes==NULL) ) return(-4);

	i=cherche_colonne(table, nomColonne);
	if(i<0){
		return(-5);
	}

	j=1; /* numéro de ligne */
	ligne=table->lignes;

	if(ligne==NULL) return(-6);

	while(j<numLigne){
		ligne=ligne->next;
		if(ligne==NULL) return(-6);
		j++;
	}

	strncpy(valeur, ligne->valeurs[i], 100-1);
	valeur[100-1]='\0';

	return(0);
}


table_csv_t *cree_table(char **entetes, int nbCol){
	
	table_csv_t *table; /*la valeur de retour */
	int i; /* compteur */

	if((entetes==NULL)||(nbCol==0)) return(NULL);

	table = malloc(sizeof(table_csv_t));
	if(table==NULL) return(NULL);

	table->nbCol=nbCol;
	table->nbLig=0;

	table->entetes=malloc(nbCol*sizeof(char*));
	if(table->entetes==NULL){
		destroy_table_csv(table);
		return(NULL);
	}

	for(i=0; i<nbCol; i++){
		if(entetes[i]==NULL) table->entetes[i]=NULL;
		else {
			table->entetes[i]=malloc((strlen(entetes[i])+1)*sizeof(char));

			if(table->entetes[i]==NULL){
				destroy_table_csv(table);
				return(NULL);
			}

			strcpy(table->entetes[i], entetes[i]);
		}
	}

	table->lignes=NULL;

	return(table);
}


int ajoute_ligne(table_csv_t *table, char **contenu, int nbContenu){

	ligne_csv_t *nouvelle, *derniere; /* lignes de la table */
	int i; /* compteur */

	if(table==NULL) return(-1);
	if(contenu==NULL) return(-2);
	if(nbContenu>table->nbCol) return(-3);

	derniere=table->lignes;
	if(derniere!=NULL) {
		while(derniere->next!=NULL) derniere=derniere->next;
	}

	nouvelle=malloc(sizeof(ligne_csv_t));
	if(nouvelle==NULL) return(-4);

	nouvelle->next=NULL;
	nouvelle->valeurs=malloc(table->nbCol*sizeof(char *));
	if(nouvelle->valeurs==NULL){
		free(nouvelle);
		return(-5);
	}
	
	for(i=0; i<table->nbCol; i++){
		if((i<nbContenu)&&(contenu[i]!=NULL)){
			nouvelle->valeurs[i]=malloc((strlen(contenu[i])+1)*sizeof(char));
			if(nouvelle->valeurs[i]==NULL){
				detruit_ligne_csv(nouvelle, i);
				return(-6);
			}
			strcpy(nouvelle->valeurs[i], contenu[i]);

		} else nouvelle->valeurs[i]=NULL;
	}

	if(derniere==NULL) table->lignes=nouvelle;
	else derniere->next=nouvelle;
	table->nbLig=table->nbLig+1;
	return(0);
}


void affiche_table(table_csv_t *table, FILE *flux){

	const char vertical='-';
	const char horizontal='|';
	const int largeurCol=10;

	int i,j; /* compteurs */
	char contenu[largeurCol-2]; /* contenu de cellule tronqué */
	char format[20]; /* formatage de chaines */
	ligne_csv_t *ligne; /* parcourt des lignes */

	if(table==NULL){
		fprintf(stderr, "affiche_table(): Pas de table à afficher\n");
		return;
	}


	/* bordure supérieure */
	fprintf(flux, "%c", horizontal);
	for(i=0; i<table->nbCol; i++){
		for(j=0; j<largeurCol; j++)
			fprintf(flux, "%c", vertical);
		fprintf(flux, "%c", horizontal);
	}
	fprintf(flux, "\n");

	/* entetes */
	fprintf(flux, "%c", horizontal);
	for(i=0; i<table->nbCol; i++){
		strncpy(contenu, table->entetes[i], largeurCol-3);
		contenu[largeurCol-3]='\0';
		sprintf(format, " %%%ds %%c", largeurCol-2);
		fprintf(flux, format, contenu, horizontal);
	}
	fprintf(flux, "\n");

	/* bordure 2 */
	fprintf(flux, "%c", horizontal);
	for(i=0; i<table->nbCol; i++){
		for(j=0; j<largeurCol; j++)
			fprintf(flux, "%c", vertical);
		fprintf(flux, "%c", horizontal);
	}
	fprintf(flux, "\n");

	/* les lignes */
	ligne=table->lignes;
	while(ligne!=NULL){
		fprintf(flux, "%c", horizontal);
		for(i=0; i<table->nbCol; i++){
			if(ligne->valeurs[i]!=NULL){
				strncpy(contenu, ligne->valeurs[i], largeurCol-3);
				contenu[largeurCol-3]='\0';
			} else contenu[0]='\0';
			sprintf(format, " %%%ds %%c", largeurCol-2);
			fprintf(flux, format, contenu, horizontal);
		}
		fprintf(flux, "\n");

		ligne=ligne->next;

		/* bordure inférieure */
		fprintf(flux, "%c", horizontal);
		for(i=0; i<table->nbCol; i++){
			for(j=0; j<largeurCol; j++)
				fprintf(flux, "%c", vertical);
			fprintf(flux, "%c", horizontal);
		}
		fprintf(flux, "\n");

	}
}


int tri_table_decroissant(table_csv_t *table, const char *nomColonne){
	int n; /* indice de la colonne de tri */
	int i, j/*, k*/; /* compteurs */
	ligne_csv_t **liste, **tri, **tempo; /* tableaux de pointeurs sur les lignes csv à trier */
	ligne_csv_t *courant; /* parcourt de la liste chainée */
	char *valeur1, *valeur2; /* les éléments à comparer */


	if(table==NULL) return(-1);
	if(nomColonne==NULL) return(-2);

	n=cherche_colonne(table, nomColonne);
	if(n<0) return(-3);

	//affiche_table(table, stdout);
	if(table->nbLig==0) return(-4);

	/* allocation des tableaux */
	liste=malloc(sizeof(ligne_csv_t *) * table->nbLig);
	if(liste==NULL) return(-5);
	
	tri=malloc(sizeof(ligne_csv_t *) * table->nbLig);
	if(tri==NULL) {
		free(liste);
		return(-6);
	}

	tempo=malloc(sizeof(ligne_csv_t *) * table->nbLig);
	if(tempo==NULL) {
		free(liste);
		free(tri);
		return(-7);
	}

	/* initialisations */
	courant=table->lignes;
	for(i = 0; i < table->nbLig; i++){
		liste[i]=courant;
		tri[i]=NULL;
		if(courant!=NULL) courant=courant->next;
	}

	/* tri */
	tri[0]=liste[0];

	for(i=1; i<table->nbLig; i++){

		/* recherche de la position de l'élément */
		for(j=0; j<i; j++){
			/* comparaison */
			valeur1=liste[i]->valeurs[n];
			valeur2=tri[j]->valeurs[n];
			if(strcmp(valeur1, valeur2)>0) break;
		}

		/* décalage éventuel du bas du tableau */
		if(j<i){
			memcpy(tempo, tri+j, sizeof(ligne_csv_t *)*(table->nbLig-j-1));
			memcpy(tri+j+1, tempo, sizeof(ligne_csv_t *)*(table->nbLig-j-1));
		}

		/* placement de l'élément*/
		tri[j]=liste[i];
	}

	/* reconstruction du chainage */
	table->lignes=tri[0];
	for(i = 0; i < table->nbLig - 1; i++){
		tri[i]->next=tri[i+1];
	}
	tri[table->nbLig - 1]->next=NULL;

	/* liberation de mémoire et fin*/
	free(liste);
	free(tri);
	free(tempo);
	return(0);
}


int fusionne_tables(table_csv_t *table1, table_csv_t *table2){
	
	int i; /* compteur */
	ligne_csv_t *derniereLigne; /* derniere ligne de la table1 */

	/* vérification des arguments */
	if(table1==NULL) return(-1);
	if(table2==NULL) return(0);
	
	/* vérification des entêtes des colonnes */
	if(table1->nbCol!=table2->nbCol) return(-1);
	
	for(i=0; i<table1->nbCol; i++){
		if(strcmp(table1->entetes[i], table2->entetes[i])) return(-2);
	}

	
	/* fusion */
	derniereLigne=table1->lignes;
	if(derniereLigne!=NULL)
		while(derniereLigne->next!=NULL) derniereLigne=derniereLigne->next;
	if(derniereLigne==NULL)
		table1->lignes=table2->lignes;
	else
		derniereLigne->next=table2->lignes;

	/* fin */
	return(0);
}


table_csv_t *selectionne_colonnes(table_csv_t *table, char **elementsCherches, int nbElementsCherches, int *nbElementsTrouves){

	int i=0; // comptage des colonnes
	int j; //compteur
	char *entete;
	table_csv_t *selection;
	int *colonnes; // les colonnes selectionnées
	char **entetes_trouves;
	ligne_csv_t *ligne; // parcours des lignes

	*nbElementsTrouves=0;

	if(table->entetes==NULL) {
		return NULL;
	}

	entetes_trouves=malloc(sizeof(char *)*nbElementsCherches);
	colonnes=malloc(sizeof(int)*nbElementsCherches);

	entete=table->entetes[i];

	while ((entete!=NULL)&&(i<table->nbCol)){

		int trouve = 0;

		for(j=0; j<nbElementsCherches; j++){
			if(!strcmp(entete, elementsCherches[j])) {
				trouve = 1;
				break;
			}
		}

		if(trouve) {
			colonnes[*nbElementsTrouves]=i;
			entetes_trouves[*nbElementsTrouves]=entete;
			(*nbElementsTrouves)++;
		}
		
		if(*nbElementsTrouves==nbElementsCherches){
			break;
		}

		i++;
		if(i<table->nbCol){
			entete=table->entetes[i];
		} else {
			entete=NULL;
		}
	}
	

	selection = cree_table(entetes_trouves, *nbElementsTrouves);

	ligne = table->lignes;

	while (ligne!=NULL){

		char **contenu = malloc (sizeof(char*) * *nbElementsTrouves);


		for(j=0; j<*nbElementsTrouves; j++){

			char *valeur = ligne->valeurs[colonnes[j]];
			contenu[j]=malloc(sizeof(char)*(strlen(valeur)+1));
			strcpy(contenu[j], valeur);
		}
		

		ajoute_ligne(selection, contenu, *nbElementsTrouves);
		
		ligne=ligne->next;
	}


	free(entetes_trouves);
	free(colonnes);
	return selection;
}


int ecrit_csv(char *nomFichier, table_csv_t *table, char separateur){
	int i, j; /*compteurs*/
	ligne_csv_t *courant; /* parcourt des lignes */
	FILE *fo; /* descripteur de fichier */

	if(nomFichier==NULL) fo=stdout;
	else fo=fopen(nomFichier, "w");
	if(fo==NULL){
		fprintf(stderr, "Impossible d'ouvrir le fichier %s", nomFichier);
		return(-1);
	}


	/*entetes*/
	for(j=0; j<table->nbCol; j++) {
		if(hasDelimiter(table->entetes[j], separateur)) {
			fprintf(fo, "\"%s\"", table->entetes[j]);
		} else {
			fprintf(fo, "%s", table->entetes[j]);
		}

		if(j<table->nbCol-1) {
			fprintf(fo, "%c", separateur);
		} else {
			fprintf(fo, "\n");
		}
	}

	/* lignes*/
	
	courant = table->lignes;
	for(i=0; i<table->nbLig; i++) {
		for(j=0; j<table->nbCol; j++) {

			if(hasDelimiter(courant->valeurs[j], separateur)){
				fprintf(fo, "\"%s\"", courant->valeurs[j]);
			} else {
				fprintf(fo, "%s", courant->valeurs[j]);
			}
			if(j<table->nbCol-1) {
				fprintf(fo, "%c", separateur);
			} else {
				fprintf(fo, "\n");
			}
		}
		
		courant=courant->next;
	}
	
	
	if(nomFichier!=NULL) fclose(fo);
	return(0);
}


int tronquer_colonne(table_csv_t *table, char *nom_colonne, int longueur){

	int n; //number of column
	int trouve; // column found?
	int i; // counter
	ligne_csv_t *ligne; // parcourt des lignes

	trouve = 0;

	for(i=0; i<table->nbCol; i++){
		if(!strcmp(nom_colonne, table->entetes[i])){
			n=i;
			trouve=1;
			break;
		}
	}
	
	if(!trouve){
		return 1;
	}

	ligne = table->lignes;
	while(ligne!=NULL){
		if(strlen(ligne->valeurs[n]) > longueur){
			ligne->valeurs[n][longueur]='\0';
		}

		ligne=ligne->next;
	}

	return 0;
}




/************************************************************************/
/*                INTERNAL FUNCTIONS IMPLEMENATATION                    */
/************************************************************************/


static void supprime_guillemets(char *chaine){
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


static void passe_en_majuscules(char *chaine){
	int i; /* counter */

	for(i=0;i<strlen(chaine);i++)
		if((chaine[i]>='a') && (chaine[i]<='z'))
			chaine[i]=chaine[i]+'A'-'a';
}


static int cherche_colonne(table_csv_t *table, const char *nomColonne){

	int n; /**< column number */
	char entete[100]; /**< a column header */
	char nomColMaj[100]; /**< the looking for column name in uercase */

	if(table==NULL) return(-1);

	if(nomColonne==NULL) return(-2);

	if( (table->nbCol==0) || (table->entetes==NULL) ) return(-3);

	strncpy(nomColMaj, nomColonne, 100-1);
	nomColMaj[100-1]='\0';
	passe_en_majuscules(nomColMaj);

	n=0;
	while(n<table->nbCol){
		strncpy(entete, table->entetes[n], 100-1);
		entete[100-1]='\0';
		passe_en_majuscules(entete);
		if(!strcmp(entete, nomColMaj)) break;
		n++;
	}

	if(n==table->nbCol) return(-1);
	return(n);
}


static char **lit_ligne(int *nbElts, FILE *fichier, char separateur){

	char **retour; /* la valeur à renvoyer */
	int nA; /* nb d'allocations de x char* */
	char ligne[TAILLE_BUF]; /* une ligne du fichier */
	int i/*,j*/,k,l,m; /*compteurs */
	char **tempo; /* tableau de chaines intermédiaire */
	int finLigne; /* indique que l'on a atteind la fin de la ligne à lire */
	char elt[100]; /* un element lu sur la ligne */
	int guillemetsOuverts; /* indique l'ouverture de guillements */
	char *err; /* retour de la fct fgets() */


	*nbElts=0;
	nA=1;
	retour=malloc(nA*100*sizeof(char*));
	if(retour==NULL){
		*nbElts=-1;
		return(NULL);
	}

	/* lecture de la ligne */
	i=0; /* indice d'élément trouvé */
	m=0; /* indice de position dans la chaine "elt" */
	finLigne=0; elt[0]='\0'; guillemetsOuverts=0;

	while(!finLigne){

		err=fgets(ligne, TAILLE_BUF, fichier);

		if(err==NULL) { /* Fin du fichier atteinte ? */
			if(i==0) {
				free(retour);
				return(NULL);
			}
			return(retour);
		}

		k=0; /* position courante sur la ligne */

		while( (k<TAILLE_BUF) && (ligne[k]!='\0') ){

			if( (!guillemetsOuverts) && (ligne[k]=='\n') ) {
				break;
			}

			/* Si séparateur trouvé ajout de l'élément */
			if( (ligne[k]==separateur) && (!guillemetsOuverts) ){
				elt[m]='\0';
				/* il peut y avoir des guillemets autour du champs */
				supprime_guillemets(elt);
				retour[i]=malloc((strlen(elt)+1)*sizeof(char));
				if(retour[i]==NULL){
					*nbElts=-2;
					return(retour);
				}
				strcpy(retour[i], elt);
				i++;
				(*nbElts)++;
				/* réinitialisation */
				elt[0]='\0'; m=0;

				/* allocation si necessaire de place supplémentaire */
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

			/* Sinon ajout du caractère au nom de l'élément */
			} else {
				if((m<100-1)&&(ligne[k]!='\r')){
					elt[m]=ligne[k];
					m++;
				}
				if(ligne[k]=='"')
					guillemetsOuverts=guillemetsOuverts?0:1;
			}

			/* passage au caractère suivant */
			k++;

		} // while ! fin de chaine ou de buffer

		/* Si on a lu tout le buffer, on doit en charger un autre */
		if(k==TAILLE_BUF) continue;

		if((ligne[k]=='\0')||(ligne[k]=='\n')) {
			/* ajout du dernier élément */
			elt[m]='\0';
			/* il peut y avoir des guillemets autour du champs */
			supprime_guillemets(elt);
			retour[i]=malloc((strlen(elt)+1)*sizeof(char));
			if(retour[i]==NULL){
				*nbElts=-2;
				return(retour);
			}
			strcpy(retour[i], elt);
			(*nbElts)++;

			/* FIN de ligne détecté */
			#ifdef DEBUG
			fprintf(stdout, "%d éléments trouvés\n", *nbElts);
			fprintf(stdout, "dernier élément : %s - %s\n", retour[i], elt);
			#endif
			finLigne=1;
		}
	} // fin de ligne


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


static void detruit_ligne_csv(ligne_csv_t *table, int nbColonnes){

	int i; // compteur
	ligne_csv_t *courant; // pour le parcourt de la table
	
	courant=table;

	while(courant!=NULL){
		if(courant->valeurs!=NULL){
			for(i=0; i<nbColonnes; i++)
				if(courant->valeurs[i]!=NULL) free(courant->valeurs[i]);
			free(courant->valeurs);
		}
		courant=courant->next;
	}
}
