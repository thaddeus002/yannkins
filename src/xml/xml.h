/**
 * @file xml.h
 * @brief XML simple utilities
 */



#ifndef _YANNKINS_XML_H__
#define _YANNKINS_XML_H__


#include <stdio.h>


/** @brief a xmlNode is a base XML element/tag */
typedef struct xmlNode_ xmlNode;
/**
 * @brief a xmlAttribute is define in the tag between '<' and '>'
 *
 * It is in the form key="value"
 */
typedef struct xmlAttribute_ xmlAttribute;


struct xmlAttribute_ {
    /** name of the attribute */
    char *key;
    /** value of the attribute */
    char *value;
    /** a tag may contents many attributes, so they are linked */
    xmlAttribute *next;
};


/**
 * The node contents :
 * <ul>
 * <li>a name and a list of attributes in the open tag (between '<' and '>')
 * <li>a text : data content between open and close tags
 * <li>a list of children nodes
 * </ul>
 * If there are text contents between the children nodes, to take them in account
 * we need to add an element to this struct model.
 * So we add a "postText" to this structure. The postText belong to parent node, and is
 * positionned after the this node.
 */
struct xmlNode_ {
    /** the tag name */
    char *name;
    /** a linked list of attributes */
    xmlAttribute *attributes;
    /** the data content of the tag */
    char *text;
    /** the children nodes */
    xmlNode *children;
    /** the next brother node (at the same depth) */
    xmlNode *next;
    /** the text after the node */
    char *postText;
};


/**
 * @brief Read an XML file
 * @param filename the name of file to read
 * @return the data under xmlNode format
 */
xmlNode *read_xml_file(char *filename);


/**
 * @brief Write a XML document in a stream
 * @param fd the stream where write
 * @param document the data
 * @param depth for the indentation
 * @return 0 if the function was successfull
 */
int write_xml_node(FILE *fd, xmlNode *document, int depth);


/**
 * @brief Write a XML document in a file
 * @param filename the name of file to write
 * @param document the data
 * @return 0 if the function was successfull
 */
int write_xml_node_in_file(char *filename, xmlNode *document);


/**
 * @brief Free memory allocated for a xmlNode
 * @param document the data to free
 */
void destroy_xmlNode(xmlNode *document);




#endif
