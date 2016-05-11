/**
 * @file data.c
 * @brief data management
 */



#include "data.h"
#include <string.h>
#define REVISION "revision"
#define AUTHOR "author"
#define DATE "date"
#define MSG "msg"



/**
 * @brief Management of svn log.
 *
 * The command "svn log --xml" returns a XML file of this form :
 * <?xml>
 * <log>
 * <logentry revision="3273">
 * <author>afletdinov</author>
 * <date>2015-01-22T01:48:58.267943Z</date>
 * <msg>fix: save old format</msg>
 * </logentry>
 * ...
 * </log>
 *
 * To exploit the information we can put them in a table of the form
 * revision;author;data;msg
 * 3273;afletdinov;2015-01-22T01:48:58.267943Z;fix: save old format
 *
 * To make this there are two actions :
 *    1) convert xml to csv
 *    2) replace ';' characters in msg fields
 *
 */
table_csv_t *present_svn_log(xmlNode *logDocument){

    table_csv_t *table;
    char *headers[4];
    xmlNode *logentry;

    
    headers[0]=REVISION;
    headers[1]=AUTHOR;
    headers[2]=DATE;
    headers[3]=MSG;

    table = cree_table(headers, 4);

    logentry = logDocument->children;

    while(logentry != NULL) {

        if ((logentry->name != NULL) && (!strcmp(logentry->name, "logentry")) ) {

            char *content[4];
            xmlAttribute *attribute = logentry->attributes;
            xmlNode *child = logentry->children;

            content[0] = NULL;
            content[1] = NULL;
            content[2] = NULL;
            content[3] = NULL;

            while(attribute!=NULL){
                if(!strcmp(attribute->key, REVISION)){
                    content[0] = attribute->value;
                    break;
                }
                attribute = attribute->next;
            }

            while(child!=NULL){

                if(child->name != NULL) {
                    if(!strcmp(child->name, AUTHOR)){
                        content[1] = child->text;
                    } else if(!strcmp(child->name, DATE)){
                        content[2] = child->text;
                    } else if(!strcmp(child->name, MSG)){
                        content[3] = child->text;
                    }
                }

                child = child->next;
            }

            ajoute_ligne(table, content, 4);
        }

        logentry = logentry->next;
    }

    return table;
}

