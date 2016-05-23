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
    
    document = read_xml_file(XML_FILE);

    table = present_svn_log(document);

    destroy_xmlNode(document);

    write_csv_file(CSV_FILE, table, ';');

    destroy_table_csv(table);
    
    return 0;
}


