/**
 * @file xml.c
 * @brief XML simple utilities
 */

#include "xml.h"
#include <stdlib.h> // malloc()

/**
 * @brief Create a new structure with initial empty values
 */
static xmlNode *init_xmlNode() {
    xmlNode *result = malloc(sizeof(xmlNode));
    result->name = NULL;
    result->attributes = NULL;
    result->text = NULL;
    result->children = NULL;
    result->next = NULL;
    return result;
}



xmlNode *read_xml_file(char *filename) {

    xmlNode *result = init_xmlNode();

    // TODO

    return result;
}


int write_xml_node(FILE *fd, xmlNode *document, int depth) {

    /* to cross the attributes */
    xmlAttribute *attribute;
    int err = 0; // error code
    int i; // counter

    if(document == NULL) {
        return 0;
    }
    
    for (i = 0; i < depth; i++) {
    	fprintf(fd, "    ");
    }
        
    fprintf(fd, "<");
    if(document->name != NULL) {
        fprintf(fd, "%s", document->name);
    }

    attribute=document->attributes;
    while(attribute != NULL) {
        fprintf(fd, " %s=\"%s\"", attribute->key, attribute->value);
        attribute=attribute->next;
    }

    fprintf(fd, ">");
    if(document->children != NULL) {
        fprintf(fd, "\n");
    }
    
    if(document->text != NULL) {
        fprintf(fd, "%s", document->text);
    }
    
    err = write_xml_node(fd, document->children, depth+1);
    
    fprintf(fd, "</%s>\n", document->name);

    if(!err) {
        err = write_xml_node(fd, document->next, depth);
    }
    
    return err;
}



int write_xml_node_in_file(char *filename, xmlNode *document) {

    FILE *fd;
    int err;

    fd = fopen(filename, "w");

    if(fd == NULL) {
        fprintf(stderr, "Can't create file %s\n", filename);
        return 1;
    }

    err = write_xml_node(fd, document, 0);
    fclose(fd);
    
    return err;
}



void destroy_xmlNode(xmlNode *document){

    /* to cross the attributes */
    xmlAttribute *attribute;

    if(document == NULL) {
        return;
    }

    if(document->name != NULL) {
        free(document->name);
    }

    attribute=document->attributes;
    while(attribute != NULL) {
        xmlAttribute *next = attribute->next;
        if(attribute->key != NULL) free(attribute->key);
        if(attribute->value!= NULL) free(attribute->value);
        free(attribute);
        attribute=next;
    }
    
    if(document->text != NULL) {
        free(document->text);
    }

    destroy_xmlNode(document->next);
    destroy_xmlNode(document->next);

    free(document);
}

