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
/** @brief a HTML node */
typedef xmlNode htmlElement;
/** @brief Represents an HTML Table */
typedef xmlNode htmlTable;
/** @brief Represents an HTML List (<ul>) */
typedef xmlNode htmlList;

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
 * @brief add a list at the end of document's body.
 * @param document the document to modify
 * @return the added element
 */
htmlList *html_add_list(htmlDocument *document);

/**
 * @brief add a item to a list
 * @param list the list to modify
 * @param item the content of the new item
 * @return the newly created item
 */
xmlNode *html_add_list_item(htmlList *list, char *item);

/**
 * @brief Create an emty html table.
 * @param nbCol the number of columns of the new table
 * @param nbLines the number of lines of the new table
 * @param headers columns' headers
 * @return the new table
 */
htmlTable *create_html_table(int nbCol, int nbLines, char **headers);


/**
 * @brief Append a table at the end of the document.
 * @param document where append the chapter title
 * @param data the data to add
 * @return the added table
 */
htmlTable *html_add_table_from_data(htmlDocument *document, table_csv_t *data);


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
 * @return the added title node
 */
xmlNode *html_add_title(htmlDocument *document, int level, char *title);

/**
 * @brief Add a chapter title with H balise, and an horizontal line
 * under the title
 * @param document where append the chapter title
 * @param level level of the title : 1 to 5
 * @param title content enter <h> and </h> balise
 * @return the added title node
 */
xmlNode *html_add_title_with_hr(htmlDocument *document, int level, char *title);

/**
 * @brief Write a text with a hyperlink.
 * @param document where append the link
 * @param text appearing text
 * @param link url to point in the link
 * @return the new link node
 */
xmlNode *html_add_link(htmlDocument *document, char *text, char *link);

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
 * @return the new image node
 */
xmlNode *html_add_image(htmlDocument *document, char *image);

/**
 * @brief Append an image to a node of the HTML document
 * @param element the node to modify
 * @param image the image's url
 * @return the new image node
 */
xmlNode *html_add_image_in_node(htmlElement *element, char *image);



/**
 * @brief Write a text with a hyperlink in a table.
 * @param table where append the link
 * @param text appearing text
 * @param link url to point in the link
 * @param col the column index
 * @param line the line index
 * @return the newly created link node
 */
xmlNode *html_add_link_in_table(htmlTable *table, char *text, char *link, int col, int line);


/**
 * @brief Write a text with a  hyperlink.
 * @param node where append the link
 * @param text appearing text
 * @param link url to point in the link
 * @return the newly created link node
 */
xmlNode *html_add_link_in_node(xmlNode *node, char *text, char *link);


/**
 * @brief Write the date in human readable form.
 * @param document where append the date
 * @param date the data to write
 * @param col the column index
 * @param line the line index
 */
void html_add_date_in_table(htmlTable *table, time_t date, int col, int line);

/**
 * @brief Write a text in a table's cell.
 * @param document where append the date
 * @param text the data to write
 * @param col the column index
 * @param line the line index
 */
void html_set_text_in_table(htmlTable *table, char *text, int col, int line);


/**
 * @brief Append an image to the HTML document
 * @param document the document to modify
 * @param image the image's url
 * @param col the column index
 * @param line the line index
 * @return the new image node or NULL if it was not created
 */
xmlNode *html_add_image_in_table(htmlTable *table, char *image, int col, int line);


/**
 * @brief Append an image to the HTML document
 * @param document the document to modify
 * @param image the image's url
 * @param width width of image
 * @param height height of image
 * @param col the column index
 * @param line the line index
 * @return the new image node or NULL if it was not create
 */
xmlNode *html_add_image_with_size_in_table(htmlTable *table, char *image, int width, int height,  int col, int line);


/**
 * @brief Write the HTML document in a file.
 * @param document the document to write
 * @param filename the name of html file
 * @return an error code, or 0 in case of success
 */
int html_write_to_file(htmlDocument *document, char *filename);



#endif
