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
 *
 * If there are text contents between the children nodes, to take them in account
 * we need to add an element to this struct model.
 * So we add a "postText" to this structure. The postText belong to parent node, and is
 * positionned after the this node.
 *
 * The root node of a document may have a header tag as "<?xlm ...>" or "<!Doctype html>"
 */
struct xmlNode_ {
    /** only the root node may have a header */
    char *header;
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
 * @brief Create a new structure
 * @param openTag a valid node declaration
 * @return a new empty node
 */
xmlNode *xml_init_node(char *header, char *openTag);


/**
 * @brief Read an XML file
 * @param filename the name of file to read
 * @return the data under xmlNode format
 */
xmlNode *xml_read_file(char *filename);

/**
 * Add a new attribute to a node
 * @param node the node to modify
 * @param key the key of the new attribute
 * @param value the value of the new attribute
 * @return 0 in case of success
 */
int xml_add_attribute(xmlNode *node, char *key, char *value);


/**
 * @brief retrieve the value of an attribute if exist
 * @param node where to look for
 * @param key the looked for key
 * @return A newly allocate copy of attribute value or null if not found
 */
char *xml_get_attribute(xmlNode *node, char *key);



/**
 * @brief Add a child node to parent.
 * @param parent the node where add
 * @param child the node to add
 * @return 0 on success
 */
int xml_add_child(xmlNode *parent, xmlNode *child);



/**
 * @brief Write a XML document in a stream
 * @param fd the stream where write
 * @param document the data
 * @param depth for the indentation
 * @return 0 if the function was successfull
 */
int xml_write_node(FILE *fd, xmlNode *document, int depth);


/**
 * @brief Write a XML document in a file
 * @param filename the name of file to write
 * @param document the data
 * @return 0 if the function was successfull
 */
int xml_write_node_in_file(char *filename, xmlNode *document);


/**
 * @brief Free memory allocated for a xmlNode
 * @param document the data to free
 */
void xml_destroy_node(xmlNode *document);




#endif
