/**
 * \file log_analyse.h
 * \brief make treatments on logs tables.
 *
 * Can group data to make series : counts the amount of lines by ranges.
 *
 * This is a utility module to manipulate data table, grouping rows into series.
 * For example :
 *
 * in input we have :
 * x;val
 * 1;sandro
 * 2;tsabu
 * 3;oris
 *
 * in output :
 * range of x;number
 * 1-2;2 (sandro and tsabu)
 * 3-4;1 (oris)
 */

#ifndef LOG_ANALYSE_H__
#define LOG_ANALYSE_H__ 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csv/csv.h"




/**
 * \brief Process a log table to count the number of entries by month
 *
 * The input table is suppose to have a column for the date. This table
 * will be sorted and the number of row will be count for each month.
 * \param table the data to process
 * \param the name of the date column
 * \return a new table with results
 */
csv_table_t *nb_by_month(csv_table_t *table, char *date_header);



/**
 * \brief Process a table of logs to count the number of entries(i.e. commits) by month and by authors.
 *
 * Add a line even for the month with no entries.
 * \param table the data to process
 * \param date_header the name of the date column
 * \param author_header the name of the author column
 * \return a new table with results
 */
csv_table_t *nb_by_month_and_by_authors(csv_table_t *table, char *date_header, char *author_header);



/**
 * Count the number of authors in a VCS log table.
 *
 * \param vcsLogTable a table of log for the Versionning Control System
 * \return the number of authors in the given table
 */
int get_authors_number(csv_table_t *vcsLogTable);


#endif
