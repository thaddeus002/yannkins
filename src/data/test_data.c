/**
 * @file test_data.c
 * Unit test of data convertion XLM -> CSV
 */

#include "data.h"

/** input file */
#define XML_FILE "test_log.xml"
/** output file */
#define CSV_FILE "test_log.output.csv"


/**
 * Test parse svn logs in XML format (command "svn log --xml"),
 * and write them in a csv file.
 */
int main(int argc, char **argv) {

    xmlNode *document;
    table_csv_t *table = NULL;

    document = xml_read_file(XML_FILE);

    table = present_svn_log(document);

    xml_destroy_node(document);

    csv_write_file(CSV_FILE, table, ';');

    csv_destroy_table(table);

    return 0;
}


