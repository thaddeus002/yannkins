/**
 * @file csv.h
 * @brief Utilitaries functions and types to manipulates data. These data
 * can be read from or write in csv files.
 * @author Yannick Garcia
 */

#ifndef CSV_H_
#define CSV_H_

#include <stdio.h>


/** @brief Structur representing one csv line or one serie of data */
typedef struct csv_line_t_ {
    char **values; /**< @brief fields values */

    struct csv_line_t_ *next; /**< @brief to make a linked list of csv lines */
} csv_line_t;


/** @brief An entire csv file */
typedef struct {
    int nbCol; /**< @brief number of colums - Don't directly modify this value */
    int nbLig; /**< @brief number of lines - Don't directly modify this value */
    char **headers; /**< @brief columns' names */

    csv_line_t *lines; /**< @brief a linked list of lines */
} csv_table_t;



/************************************************************************/
/*                            THE FUNCTIONS                             */
/************************************************************************/


/**
 * @brief read a csv file.
 *
 * The quotes between fields are removed.
 * @param filename the name of csv file
 * @param delimiter the split character
 * @return NULL if the file can not be read
 */
 csv_table_t *csv_read_file(char *filename, char delimiter);


/**
 * @brief Create a new table with less columns.
 * @param table the origin table
 * @param searchedElts table of columns names to look for
 * @param nbSearchedElts number of elements in searchedElts
 * @param nbFoundElts the function will plce here  the number of columns found
 * @return the new table
 */
csv_table_t *csv_select_columns(csv_table_t *table, char **searchedElts, int nbSearchedElts, int *nbFoundElts);


/**
 * @brief Cut if necessary the lenght of fields in a given column.
 * @param table the data to modify
 * @param columnsName the name of the columns to strech
 * @param ltk the length to keep
 * @return 0 in case of success
 */
int csv_truncate_column(csv_table_t *table, char *columnsName, int ltk);


/**
 * @brief free memory occuped by a csv_table_t
 * @param table the struct to free
 */
void csv_destroy_table(csv_table_t *table);


/**
 * @brief select the lines with property name=value.
 * @param table the table where look for data
 * @param columnsName name of column
 * @param value value to find in the column
 * @return the new table with only the selected lines
 */
csv_table_t *csv_select_lines(csv_table_t *table, const char *columnsName, const char *value);


/**
 * @brief select the lines with property min<=value<=max.
 * @param table the table where look for data
 * @param columnsName name of column
 * @param min the minimun value for the column
 * @param max the maximun value for the column
 * @return the new table with only the selected lines
 */
csv_table_t *csv_select_lines_range(csv_table_t *table, const char *columnsName, const char *min, const char *max);


/**
 * @brief find the value at a given position.
 * @param value the function will put here the result - must be allocated
 * @param table the data
 * @param columnsName column name to look for
 * @param line line number (beginning at 1) to look for
 * @return a non null code if un error occured
 */
int csv_find_value(char value[100], csv_table_t *table, char *columnsName, int line);


/**
 * @brief create an empty data table
 * @param headers headers of columns
 * @param nbCol number of columns
 * @return the newly created table
 */
csv_table_t *csv_create_table(char **headers, int nbCol);


/**
 * @brief Add a line to csv table
 *
 * If the line to add has less elements than the number of columns, the line
 * will be complete with NULL. If there are more element an error code
 * will be reurned.
 * @param table the csv table
 * @param content the data to add
 * @param contentLength to number of elements in content
 * @return a non null code if un error occured
 */
int csv_add_line(csv_table_t *table, char **content, int contentLength);


/**
 * @brief Show the table.
 * @param table the data to print
 * @param output the stream where print the data
 */
void csv_show_table(csv_table_t *table, FILE *output);


/**
 * @brief sort the table in decreasing order for a given column
 * @param table the table to sort
 * @param columnsName the name of the column to sort by
 * @return a non null code if un error occured
 */
int csv_sort_table_decreasing(csv_table_t *table, const char *columnsName);


/**
 * @brief merge two tables.
 *
 * The first table will be added the lines of the seconds if the headers lines are
 * identicals.
 * @param table1 the first table
 * @param table2 the second table
 * @return a non null code if un error occured
 */
int csv_merge_tables(csv_table_t *table1, csv_table_t *table2);


/**
 * @brief write a file with the content of a csv_table_t
 *
 * In case of \n or delimiter character in a field, this field will be between
 * doble quotes.
 * @param filename the name of the file to create
 * @param table the data
 * @param delimiter fields delimiter
 * @return a non null code if un error occured
 */
int csv_write_file(char *filename, csv_table_t *table, char delimiter);


#endif
