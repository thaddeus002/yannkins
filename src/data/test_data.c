/**
 * @file test_data.c
 * Unit test of data convertion XLM -> CSV
 */

#include "data.h"

#define XML_FILE "test_log.xml"
#define CSV_FILE "test_log.output.csv"


int main(int argc, char **argv) {

    xmlNode *document;
    table_csv_t *table = NULL;
    
    document = read_xml_file(XML_FILE);

    table = present_svn_log(document);

    destroy_xmlNode(document);
        
    affiche_table(table, stdout);
    ecrit_csv(CSV_FILE, table, ';');

    destroy_table_csv(table);
    
    return 0;
}


