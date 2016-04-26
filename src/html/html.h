/**
 * @file html.h
 * @brief Creation of a HTML page
 * @author Yannick Garcia
 * @date 02/08/2015
 */
 
#ifndef HTML_H_
#define HTML_H_

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include "../csv/csv.h"


/**
 * @brief Write the beginning of file (open html balise).
 * @param fd output to write
 */
void html_ecrit_ouverture(FILE *fd);

/**
 * @brief Write the end of file (close html balise).
 * @param fd output to write
 */
void html_ecrit_fermeture(FILE *fd);

/**
 * @brief Write the HTML header.
 * @param fd output to write
 * @param title the title of the page
 */
void html_ecrit_entete(FILE *fd, char *title);

/**
 * @brief Write the beginning of body (open body balise).
 * @param fd output to write
 */
void html_open_body(FILE *fd);

/**
 * @brief Write the end of body (close body balise).
 * @param fd output to write
 */
void html_close_body(FILE *fd);

/**
 * @brief Write a chapter title with H balise.
 * @param fd output to write
 * @param level level of the title : 1 to 5
 * @param title content enter <h> and </h> balise
 */
void html_write_title(FILE *fd, int level, char *title);

/**
 * @brief Write a chapter title with H balise, and an horizontal line
 * under the title
 * @param fd output to write
 * @param level level of the title : 1 to 5
 * @param title content enter <h> and </h> balise
 */
void html_write_title_with_hr(FILE *fd, int level, char *title);

/**
 * @brief Write a text with a hyperlink.
 * @param fd output to write
 * @param text appearing text
 * @param link url to point in the link
 */
void html_write_link(FILE *fd, char *text, char *link);

/**
 * @brief Write the date in human readable form.
 * @param fd where to write
 * @param date the data to write
 */
void html_ecrit_date(FILE *fd, time_t date);

/**
 * @brief write data in a table.
 * @param fd where to write
 * @param data the data to present in the HTML page
 */
void html_write_table(FILE *fd, table_csv_t *data);

#endif
