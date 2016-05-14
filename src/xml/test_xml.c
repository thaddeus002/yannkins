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

    document = read_xml_file(XML_FILE);

    write_xml_node_in_file(OUTPUT_FILE, document);

    destroy_xmlNode(document);

    sprintf(command, "[ $(diff %s %s | wc -l) -eq 0 ]", XML_FILE, OUTPUT_FILE);
    err = system(command);
    fprintf(stdout, "%s -> %d\n", command, err);

    if(err>=256) { err = 1; }

    return err;
}
