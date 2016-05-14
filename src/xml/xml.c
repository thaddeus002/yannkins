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


/** Allocate memory and copy the content of a given string */
static char *copyString(char *object) {
    char *copy = malloc(sizeof(char) * (strlen(object) + 1));
    strcpy(copy, object);
    return copy;
}


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
 * Replace multiples spaces,LF,CR,... by an unique space.
 */
static char *clean_open_tag(char *openTag) {

    char *result = malloc(sizeof(char) * (strlen(openTag)+1));
    int i, j;
    int space = 0;

    j=0;
    for(i=0; i<strlen(openTag); i++) {

        if(!isspace(openTag[i])){
            result[j] = openTag[i];
            j++;
            space = 0;
        } else {
            if(!space){
                result[j] = ' ';
                j++;
                space = 1;
            }
        }
    }

    result[j] = '\0';
    return result;
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
    char *value = NULL;
    int closed = 0; // end reatched
    int pos = 1; // reading position in openTag
    char *tag;

    result->name = NULL;
    result->attributes = NULL;

    // analyse of openTag
    tag = clean_open_tag(openTag);
    name = tag + 1;

    while(!closed) {
        while((tag[pos] != ' ')&&(tag[pos] != '=')&&(tag[pos] != '>')) {
            pos++;
        }

        switch(tag[pos]){
        case ' ' :
            tag[pos]='\0';
            if(name!=NULL){
                result->name = malloc(sizeof(char) * (strlen(name) + 1));
                strcpy(result->name, name);
                name = NULL;
            } else {
                if(key != NULL) {
                    addAttribute(result, key, value);
                }
            }

            pos++;
            while(isspace(tag[pos])) { pos++; }
            key=tag+pos;
            break;
        case '=' :
            tag[pos]='\0';
            value=tag+pos+1;
            pos++;
            break;
        case '>' :
            closed = 1;
            tag[pos]='\0';
            if(name!=NULL){
                result->name = malloc(sizeof(char) * (strlen(name) + 1));
                strcpy(result->name, name);
            } else {
                if(key != NULL) {
                    addAttribute(result, key, value);
                }
            }
        }
    }
    free(tag);
    
    result->text = NULL;
    result->children = NULL;
    result->next = NULL;
    result->postText = NULL;
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
    // Suppress line feed, carriage return, and spaces before element
    while(isspace(c)) { c=fgetc(fd); }
    
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
                i--;
            }
            break;
        case TAG:
            if(c=='>') {
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
 * @return the last child of a node or NULL if the node has no child
 */
static xmlNode *lastChild(xmlNode *node){

    xmlNode *last = NULL;

    if(node==NULL) { return NULL; }
    
    if(node->children == NULL) { return NULL; }

    last = node->children;

    while(last->next != NULL) { last = last->next; }

    return last;
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

        if(isContent(object)) {
            xmlNode *child = lastChild(result);
            if(child == NULL) {
                result->text = copyString(object);
            } else {
                child->postText = copyString(object);
            }
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
        for (i = 0; i < depth+1; i++) {
            fprintf(fd, "    ");
        }
        fprintf(fd, "%s", document->text);
    }
    
    err = write_xml_node(fd, document->children, depth+1);

    for (i = 0; i < depth; i++) {
            fprintf(fd, "    ");
        }
    fprintf(fd, "</%s>\n", document->name);

    if(document->postText != NULL) {
        // This could only occur when depth > 0
        for (i = 0; i < depth; i++) {
            fprintf(fd, "    ");
        }
        fprintf(fd, "%s", document->postText);
    }


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

    destroy_xmlNode(document->children);
    destroy_xmlNode(document->next);

    if(document->postText != NULL) {
        free(document->postText);
    }

    free(document);
}

