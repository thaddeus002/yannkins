/**
 * @file test_xml.c
 * Unit test of xml parsing
 */

#include "xml.h"
#include <stdlib.h>
#include <stdio.h>

#define XML_FILE "test.xmlInput.xml"
#define OUTPUT_FILE "output.xml.tmp"


/** Will return 0 on success */
int main(int argc, char **argv) {

    xmlNode *document;
    int err;
    char command[200];

    fprintf(stdout, "Reading test document\n");
    document = xml_read_file(XML_FILE);

    fprintf(stdout, "Writing test document in %s\n", OUTPUT_FILE);
    xml_write_node_in_file(OUTPUT_FILE, document);

    fprintf(stdout, "Freeing memory\n");
    xml_destroy_node(document);

    fprintf(stdout, "Comparing input and output\n");
    sprintf(command, "[ $(diff %s %s | wc -l) -eq 0 ]", XML_FILE, OUTPUT_FILE);
    err = system(command);
    fprintf(stdout, "%s -> %d\n", command, err);

    if(err>=256) { err = 1; }

    fprintf(stdout, "XML tests completed\n");
    return err;
}
