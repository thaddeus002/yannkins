/**
 * @file html.c
 * @brief Creation of a HTML page
 * @author Yannick Garcia
 * @date 02/08/2015
 */

#include <stdio.h>
#include "html.h"

/**
 * @brief old header of HTML page
 * @deprecated use HTML5_HEADER instead
 */
#define HTML4_HEADER "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">"
/** @brief Header to use in pages */
#define HTML5_HEADER "<!DOCTYPE html>"


void html_ecrit_ouverture(FILE *fd){
	fprintf(fd, "%s\n", HTML5_HEADER);
	fprintf(fd, "<html>\n");
}


void html_ecrit_fermeture(FILE *fd){
	fprintf(fd, "</html>\n");
}


void html_ecrit_entete(FILE *fd, char *title){
	fprintf(fd, "<head>\n");
	fprintf(fd, "\t<title>%s</title>\n\t<link rel=\"stylesheet\" href=\"style/style.css\">\n", title);
	fprintf(fd, "\t<meta charset=\"utf-8\">\n</head>\n");
}


void html_open_body(FILE *fd){
	fprintf(fd, "<body>\n");
}


void html_close_body(FILE *fd){
	fprintf(fd, "</body>\n");
}

// internal function
static void html_write_title_with_hr_or_not(FILE *fd, int level, char *title, int with_hr){
	fprintf(fd, "<h%d>%s</h%d>", level, title, level);
	if(with_hr){
		fprintf(fd, "<hr/>");
	}
	fprintf(fd, "\n");
}


void html_write_title(FILE *fd, int level, char *title){
	html_write_title_with_hr_or_not(fd, level, title, 0);
}


void html_write_title_with_hr(FILE *fd, int level, char *title){
	html_write_title_with_hr_or_not(fd, level, title, 1);
}


void html_write_link(FILE *fd, char *text, char *link){
	fprintf(fd, "<a href=%s>%s</a>\n", link, text);
}


void html_ecrit_date(FILE *fd, time_t date){

	struct tm time;

	localtime_r(&date, &time);

	fprintf(fd, "%02d/%02d/%04d %02d:%02d", time.tm_mday, time.tm_mon + 1, time.tm_year + 1900, time.tm_hour, time.tm_min); 
}


void html_write_table(FILE *fd, table_csv_t *data){

	int i, j; // counters
	char *value; // string to write in output stream
	ligne_csv_t *ligne; // a line of data

	// headers line
	fprintf(fd, "<table>\n\t<thead>\n\t\t<tr>\n");

	i=0;
	value = data->entetes[0];

	while ( ( i < data->nbCol ) && ( value != NULL ) ) {
		fprintf(fd, "\t\t\t<th>%s</th>\n", value);
		i++;
		value=data->entetes[i];
	}

	fprintf(fd, "</tr>\n\t</thead>\n\t<tbody>\n");

	i=0;
	ligne=data->lignes;
	while ((i<data->nbLig) && (ligne!=NULL)){
		
		fprintf(fd, "\t\t<tr>\n");

		j=0;
		value = ligne->valeurs[0];

		while ((j < data->nbCol)&&(value!=NULL)){
			fprintf(fd, "\t\t\t<td>%s</td>\n", value);

			j++;
			value=ligne->valeurs[j];
		}
		fprintf(fd, "\t\t</tr>\n");

		i++;
		ligne=ligne->next;
	}

	// ends table
	fprintf(fd, "\t</tbody>\n</table>\n");
}

