/**
 * @file convert_svn_log_from_xml_to_csv.c
 */

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * \brief the usage function
 * \param prog name of the programm
 */
void usage(char *prog) {
    fprintf(stderr, "Usage: %s [ -h ] -i <inputFile> -o <outputFile>\n", prog);
    exit(1);
}

/**
 * Test parse svn logs in XML format (command "svn log --xml"),
 * and write them in a csv file.
 */
int main(int argc, char **argv) {

    xmlNode *document;
    csv_table_t *table = NULL;
    int c;
    char *inputFile = NULL;
    char *outputFile = NULL;

    /* reading arguments */
    while ((c = getopt( argc, argv, "ioh" )) != EOF) {
        switch(c){
            case 'i' :
                inputFile = argv[optind];
                break;
            case 'o' :
                outputFile = argv[optind];
                break;
            default:
                usage(argv[0]);
        }
    }

    if(inputFile == NULL || outputFile == NULL) {
        usage(argv[0]);
    }

    document = xml_read_file(inputFile);

    if(document == NULL) {
        fprintf(stderr, "An error occurred, abort.\n");
        exit(2);
    }

    table = present_svn_log(document);

    xml_destroy_node(document);

    if(csv_write_file(outputFile, table, ';')) {
        fprintf(stderr, "Warning : Output file not created.\n");
    }

    csv_destroy_table(table);

    return 0;
}


