/**
 * @file data.h
 * @brief data management
 */



#ifndef _YANNKINS_DATA_H__
#define _YANNKINS_DATA_H__

#include "../csv/csv.h"
#include "../xml/xml.h"

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
table_csv_t *present_svn_log(xmlNode *logDocument);










#endif
