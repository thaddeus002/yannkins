/**
 * @file data.c
 * @brief data management
 */



#include "data.h"
#include "../csv/utils.h"
#include <string.h>
#include <stdlib.h>


#define REVISION "revision"
#define AUTHOR "author"
#define DATE "date"
#define MSG "msg"




/**
 * @brief Remove non significant characters in date string.
 *
 * UTC date will be consider as local date.
 * ex.: "2016-03-21T01:25:43.949240Z" will become "2016-03-21 01:25:43"
 * @return a new date
 */
char *clean_date_format(char *date){
    char *result;

    if(date == NULL) { return NULL; }

    result = malloc(sizeof(char) * (strlen(date) + 1));

    if(result == NULL) { return NULL; }

    strcpy(result, date);

    if(strlen(result)>=11) {
        result[10]=' ';
    }

    if(strlen(result)>=20) {
        result[19]='\0';
    }

    return result;
}




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
csv_table_t *present_svn_log(xmlNode *logDocument){

    csv_table_t *table;
    char *headers[4];
    xmlNode *logentry;


    headers[0]="#";
    headers[1]=AUTHOR;
    headers[2]=DATE;
    headers[3]="commentaries";

    table = csv_create_table(headers, 4);

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
                    content[0] = suppress_quotes_new_string(attribute->value);
                    break;
                }
                attribute = attribute->next;
            }

            while(child!=NULL){

                if(child->name != NULL) {
                    if(!strcmp(child->name, AUTHOR)){
                        content[1] = child->text;
                    } else if(!strcmp(child->name, DATE)){
                        content[2] = clean_date_format(child->text);
                    } else if(!strcmp(child->name, MSG)){
                        content[3] = child->text;
                    }
                }

                child = child->next;
            }

            csv_add_line(table, content, 4);
            free(content[0]);
            free(content[2]);
        }

        logentry = logentry->next;
    }

    return table;
}

