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
#include "../xml/xml.h"

/** @brief A HTML Document is a XML document */
typedef xmlNode htmlDocument;
/** @brief Represents an HTML Table */
typedef xmlNode htmlTable;


/**
 * @brief Create a new HTML page.
 * @param title the document's title
 * @return the newly created document
 */
htmlDocument *create_html_document(char *title);

/**
 * @brief Free the memory used by an HTML document.
 * @param document the structure to free
 */
void destroy_html_document(htmlDocument *document);

/**
 * @brief add a stylesheet in the document's header
 * @param document the document to modify
 */
void html_add_css(htmlDocument *document, char *cssFile);

/**
 * @brief add content at the end of document's body.
 * @param document the document to modify
 * @param data the data to add
 */
void html_add_data(htmlDocument *document, xmlNode *data);

/**
 * @brief Create an emty html table.
 * @param nbCol the number of columns of the new table
 * @param nbLines the number of lines of the new table
 */
htmlTable *create_html_table(int nbCol, int nbLines);

/**
 * @brief Free the memory use by a table.
 * @param table the struct ti free 
 */
void destroy_html_table(htmlTable *table);

/**
 * @brief Append a table at the end of the document.
 * @param document where append the chapter title
 * @param data the data to add
 */
 void html_add_table_from_data(htmlDocument *document, table_csv_t *data);

/**
 * @brief Append a table at the end of the document.
 * @param document where append the chapter title
 * @param table the data to add
 */
void html_add_table(htmlDocument *document, htmlTable *table);

/**
 * @brief Add a chapter title with H balise.
 * @param document where append the chapter title
 * @param level level of the title : 1 to 5
 * @param title content enter <h> and </h> balise
 */
void html_add_title(htmlDocument *document, int level, char *title);

/**
 * @brief Add a chapter title with H balise, and an horizontal line
 * under the title
 * @param document where append the chapter title
 * @param level level of the title : 1 to 5
 * @param title content enter <h> and </h> balise
 */
void html_add_title_with_hr(htmlDocument *document, int level, char *title);

/**
 * @brief Write a text with a hyperlink.
 * @param document where append the link
 * @param text appearing text
 * @param link url to point in the link
 */
void html_add_link(htmlDocument *document, char *text, char *link);

/**
 * @brief Write the date in human readable form.
 * @param document where append the date
 * @param date the data to write
 */
void html_add_date(htmlDocument *document, time_t date);

/**
 * @brief Append an image to the HTML document
 * @param document the document to modify
 * @param image the image's url
 */
void html_add_image(htmlDocument *document, char *image);

/**
 * @brief Write a text with a hyperlink.
 * @param document where append the link
 * @param text appearing text
 * @param link url to point in the link
 * @param col the column index
 * @param line the line index
 */
void html_add_link_in_table(htmlTable *table, char *text, char *link, int col, int line);

/**
 * @brief Write the date in human readable form.
 * @param document where append the date
 * @param date the data to write
 * @param col the column index
 * @param line the line index
 */
void html_add_date_in_table(htmlTable *table, time_t date, int col, int line);

/**
 * @brief Append an image to the HTML document
 * @param document the document to modify
 * @param image the image's url
 * @param col the column index
 * @param line the line index
 */
void html_add_image_in_table(htmlTable *table, char *image, int col, int line);

/**
 * @brief Write the HTML document in a file.
 * @param document the document to write
 * @param filename the name of html file
 * @return an error code, or 0 in case of success
 */
int html_write_to_file(htmlDocument *document, char *filename);








// Deprecated API

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
