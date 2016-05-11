/**
 * @file xml.c
 * @brief XML simple utilities
 */

#include "xml.h"
#include <stdlib.h> // malloc()
#include <string.h> // strlen(), ...
#include <ctype.h> // isspace()

/** Reading buffer size */
#define BUF_SIZE 1024

/** Type of XML object */
typedef enum obj_type {
    UNDEFINED,
    TEXT, /**< Text inside the tag. ex.: between <p> and </p> */
    TAG /**< tag ex.: <p> */
} objType;

/**
 * Add a new attribute to a node
 * @param node the node to modify
 * @param key the key of the new attribute
 * @param value the value of the new attribute
 * @return 0 in case of success
 */
static int addAttribute(xmlNode *node, char *key, char *value){
    
    xmlAttribute *last = node->attributes;
    xmlAttribute *new = malloc(sizeof(xmlAttribute));

    new->key = malloc(sizeof(char) * (strlen(key)+1));
    strcpy(new->key, key);
    new->value = malloc(sizeof(char) * (strlen(value)+1));
    strcpy(new->value, value);
    new->next = NULL;

    if(last==NULL) {
        node->attributes = new;
    } else {
        while(last->next != NULL) { last = last->next; }
        last->next = new;
    }

    return 0;
}


/**
 * @brief Add a child node to parent.
 */
static int addChild(xmlNode *parent, xmlNode *child){

    xmlNode *last = parent->children;

    if(last == NULL) {
        parent->children = child;
    } else {
        while(last->next!=NULL) { last = last->next; }
        last->next = child;
    }

    return 0;
}


/**
 * @brief Create a new structure
 * @param openTag a valid node declaration
 * @return a new empty node
 */
static xmlNode *init_xmlNode(char *openTag) {
    xmlNode *result = malloc(sizeof(xmlNode));
    char *name = NULL;
    char *key = NULL;
    char *value =NULL;
    int closed = 0; // end reatched
    int pos = 1; // reading position in openTag

    result->name = NULL;
    result->attributes = NULL;

    // analyse of openTag
    name = openTag + 1;

    while(!closed) {
        while((openTag[pos] != ' ')&&(openTag[pos] != '=')&&(openTag[pos] != '>')) {
            pos++;
        }

        switch(openTag[pos]){
        case ' ' :
            openTag[pos]='\0';
            if(name!=NULL){

                result->name = malloc(sizeof(char) * (strlen(name) + 1));
                strcpy(result->name, name);
                name = NULL;
            } else {
                if(key != NULL) {
                    addAttribute(result, key, value);
                }
            }

            while(isspace(openTag[pos])) { pos++; }
            key=openTag+pos;
            break;
        case '=' :
            openTag[pos]='\0';
            value=openTag+pos+1;
            break;
        case '>' :
            closed = 1;
        }
    }
    
    result->text = NULL;
    result->children = NULL;
    result->next = NULL;
    return result;
}


/**
 * @brief Read the next elementary object in the stream
 * 
 * An elementary object is a string of text (content of a tag), or a text between braces
 * '<' and '>' (a tag).
 * @return the next elementary object in the stream or NULL if end of file is reatched
 */
static char *read_elementary_object(FILE *fd) {

    char *obj = NULL;
    int objSize=0;
    int i=0; // number of chars read
    int c; // read charcater
    objType type = UNDEFINED; 
    int end=0; // to quit loop

    c=fgetc(fd);
    i++;
    while(c!=EOF && !end) {

        int addChar = 1;

        switch(type) {
        case UNDEFINED: // first character
            if(c=='<') {
                type = TAG;
            } else {
                type = TEXT;
            }
            break;
        case TEXT:
            if(c=='<') {
                end = 1;
                addChar = 0;
                ungetc(c, fd);
            }
            break;
        case TAG:
            if(c=='<') {
                end = 1;
            }
            break;
        default:
            break;
        }


        if(addChar) {
            // add c to obj

            if (i > objSize-1) {
                // need realloc
                if(obj==NULL) {
                    obj=malloc(sizeof(char)*BUF_SIZE);
                    objSize=BUF_SIZE;
                } else {
                    obj=realloc(obj, objSize+BUF_SIZE);
                    objSize+=BUF_SIZE;
                }
            }

            obj[i-1]=c;
        }

        if(!end) { c=fgetc(fd); i++; }
    }

    obj[i]='\0';
    return obj;
}



/**
 * @return 1 if the string is the beginning of a node, 0 otherwise
 */
static int isOpenTag(char *object) {

    int lenght;

    if(object == NULL) { return 0; }

    lenght = strlen(object);

    if(lenght < 3) { return 0; }

    if( (object[0] != '<') || (object[lenght-1]!='>') || (object[1]=='?') || (object[1]=='/') ) { return 0; }

    return 1;
}


/**
 * @return 1 if the string is the ending of a node, 0 otherwise
 */
static int isCloseTag(char *object) {
    int lenght;

    if(object == NULL) { return 0; }

    lenght = strlen(object);

    if(lenght < 3) { return 0; }

    if( (object[0] != '<') || (object[lenght-1]!='>') || (object[1]=='?') ) { return 0; }

    if( (object[1] != '/') && (object[lenght-2]!='/') ) { return 0; }

    return 1;
}


/**
 * @return 1 if the string is the content of a node, 0 otherwise
 */
static int isContent(char *object) {
    if(object == NULL) { return 0; }
    if(object[0] == '\0') { return 0; }
    if(object[0] == '<') { return 0; }
    return 1;
}

/**
 * @brief Read an XML node in a stream
 * @param fd stream where read
 * @param openTag the declaration tag (already read)
 * @return a pointer to the newly created node
 */
static xmlNode *read_xmlNode_in_stream(FILE *fd, char *openTag){
    xmlNode *result = init_xmlNode(openTag);
    char *object;
    int endOfNode = 0;

    if (isCloseTag(openTag)) { endOfNode = 1; }

    while(!endOfNode) {

        object = read_elementary_object(fd);

        if( (object == NULL) || (strlen(object)==0) ) {
            if(object != NULL) free (object);
            break;
        }

        if( isContent(object) ) {
            result->text = malloc(sizeof(char) * (strlen(object + 1)));
            strcpy(result->text, object);
        } else if(isOpenTag(object)) {
            xmlNode *child = read_xmlNode_in_stream(fd, object);
            addChild(result, child);
        } else if (isCloseTag(object)) {
            endOfNode = 1;
        }

        free(object);
    }
    
    return result;
}



/**
 * @brief Read an XML node in a stream
 */
static xmlNode *read_xmlDoc_in_stream(FILE *fd){
    xmlNode *result = NULL;
    char *object;

    object = read_elementary_object(fd);
   
    while( (!isOpenTag(object)) && (object != NULL)){
        free(object);
        object = read_elementary_object(fd);
    }

    if(object != NULL) {
        result = read_xmlNode_in_stream(fd, object);
        free(object);
    }
    
    return result;
}




xmlNode *read_xml_file(char *filename) {

    xmlNode *result = NULL;

    FILE *fd;
    int err;

    fd = fopen(filename, "r");

    if(fd == NULL) {
        fprintf(stderr, "Can't read file %s\n", filename);
        return NULL;
    }

    result = read_xmlDoc_in_stream(fd);
    fclose(fd);
    
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

