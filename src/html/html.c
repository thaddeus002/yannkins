/**
 * @file html.c
 * @brief Creation of a HTML page
 * @author Yannick Garcia
 * @date 02/08/2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html.h"

/**
 * @brief old header of HTML page
 * @deprecated use HTML5_HEADER instead
 */
#define HTML4_HEADER "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">"
/** @brief Header to use in pages */
#define HTML5_HEADER "<!DOCTYPE html>"

#define STRDUP(x) x!=NULL?strdup(x):NULL


htmlDocument *html_create_document(char *title){

    htmlDocument *doc = xml_init_node(HTML5_HEADER, "<html>");
    xmlNode *head = xml_init_node(NULL, "<head>");
    xmlNode *body = xml_init_node(NULL, "<body>");
    xmlNode *htitle = xml_init_node(NULL, "<title>");
    xmlNode *meta = xml_init_node(NULL, "<meta>");

    htitle->text = STRDUP(title);
    xml_add_attribute(meta, "charset", "utf-8");

    xml_add_child(doc, head);
    xml_add_child(head, htitle);
    xml_add_child(head, meta);
    xml_add_child(doc, body);

    return doc;
}


void html_destroy_document(htmlDocument *document) {
    xml_destroy_node(document);
}


void html_add_css(htmlDocument *document, char *cssFile) {

    xmlNode *stylesheet = xml_init_node(NULL, "<link>");
    xmlNode *htmlChild;

    xml_add_attribute(stylesheet, "rel", "stylesheet");
    xml_add_attribute(stylesheet, "href", cssFile);

    htmlChild=document->children;

    while(htmlChild!=NULL) {
        if(!strcmp(htmlChild->name, "head")) {
            xml_add_child(htmlChild, stylesheet);
            break;
        }
        htmlChild = htmlChild->next;
    }
}


void html_add_data(htmlDocument *document, xmlNode *data){

    xmlNode *htmlChild = document->children;

    while(htmlChild!=NULL) {
        if(!strcmp(htmlChild->name, "body")) {
            xml_add_child(htmlChild, data);
            break;
        }
        htmlChild = htmlChild->next;
    }
}


htmlList *html_add_list(htmlDocument *document) {
    htmlList *list = xml_init_node(NULL, "<ul>");

    html_add_data(document, list);
    return list;
}


xmlNode *html_add_list_item(htmlList *list, char *item){
    xmlNode *hitem = xml_init_node(NULL, "<li>");
    hitem->text = STRDUP(item);
    xml_add_child(list, hitem);
    return hitem;
}


htmlTable *html_create_table(int nbCol, int nbLines, char **headers){
    xmlNode *table = xml_init_node(NULL, "<table>");
    int i, j;
    xmlNode *tbody = xml_init_node(NULL, "<tbody>");


    if(headers != NULL) {

        xmlNode *thead = xml_init_node(NULL, "<thead>");
        xmlNode *htr = xml_init_node(NULL, "<tr>");

        xml_add_child(table, thead);
        xml_add_child(thead, htr);

        for(j=0; j<nbCol; j++) {
            xmlNode *th;
            if(headers[j]==NULL) { break; }
            th = xml_init_node(NULL, "<th>");
            th->text = STRDUP(headers[j]);
            xml_add_child(htr, th);
        }
    }

    xml_add_child(table, tbody);

    for(i=1; i<=nbLines; i++) {
        xmlNode *tr = xml_init_node(NULL, "<tr>");
        xml_add_child(tbody, tr);
        for(j=1; j<=nbCol; j++) {
            xmlNode *td = xml_init_node(NULL, "<td>");
            xml_add_child(tr, td);
        }
    }

    return table;
}


htmlTable *html_add_table_from_data(htmlDocument *document, table_csv_t *data) {

    int nbCol, nbLines;
    int i, j;
    htmlTable *table;
    xmlNode *tbody;
    xmlNode *tr, *td;
    ligne_csv_t *csvLine;

    nbCol = data->nbCol;
    nbLines = data->nbLig;

    table = html_create_table(nbCol, nbLines, data->entetes);

    tbody = table->children;
    while( (tbody!=NULL) && (strcmp(tbody->name,"tbody")) ) {
        tbody = tbody->next;
    }

    if(tbody == NULL) {
        // this may not append
        fprintf(stderr, "Warning: Unexpected null pointer in function html_add_table_from_data()\n");
        xml_destroy_node(table);
        return NULL;
    }

    tr=tbody->children;
    csvLine=data->lignes;
    for(i=1; i<=nbLines; i++) {
        td=tr->children;
        for(j=1; j<=nbCol; j++) {
            td->text = STRDUP(csvLine->valeurs[j-1]);
            td=td->next;
        }
        tr=tr->next;
        csvLine=csvLine->next;
    }

    html_add_table(document, table);
    return table;
}


void html_add_table(htmlDocument *document, htmlTable *table) {
    html_add_data(document, table);
}


xmlNode *html_add_title(htmlDocument *document, int level, char *title) {
    xmlNode *htitle;
    char htag[5];
    int l = level;


    if(l>=10) { l=9; }
    if(l<=0)  { l=1; }
    sprintf(htag, "<h%d>", l);

    htitle = xml_init_node(NULL, htag);
    htitle->text = STRDUP(title);

    html_add_data(document, htitle);

    return htitle;
}


xmlNode *html_add_title_with_hr(htmlDocument *document, int level, char *title) {
    xmlNode *hr = xml_init_node(NULL, "<hr>");

    xmlNode *htitle = html_add_title(document, level, title);
    html_add_data(document, hr);

    return htitle;
}


xmlNode *html_add_link(htmlDocument *document, char *text, char *link){
    xmlNode *hlink = xml_init_node(NULL, "<a>");

    xml_add_attribute(hlink, "href", link);
    hlink->text=STRDUP(text);
    html_add_data(document, hlink);

    return hlink;
}


xmlNode *html_add_image(htmlDocument *document, char *image) {
    xmlNode *img = xml_init_node(NULL, "<img>");

    xml_add_attribute(img, "src", image);
    html_add_data(document, img);
    return img;
}


xmlNode *html_add_image_in_node(htmlElement *element, char *image){
    xmlNode *img = xml_init_node(NULL, "<img>");

    xml_add_attribute(img, "src", image);
    xml_add_child(element, img);
    return img;
}



/** Find a table cell */
static xmlNode *find_table_cell(htmlTable *table, int col, int line){
    int i;
    xmlNode *tbody;
    xmlNode *tr, *td;

    tbody = table->children;
    while( (tbody!=NULL) && (strcmp(tbody->name,"tbody")) ) {
        tbody = tbody->next;
    }

    if(tbody == NULL) {
        // this may not append
        fprintf(stderr, "Warning: Unexpected null pointer in function html_add_table_from_data()\n");
        return NULL;
    }

    tr = tbody->children;
    for(i=0; i<line; i++) {
        if(tr == NULL) { break; }
        tr=tr->next;
    }

    if(tr==NULL) { return NULL; }

    td = tr->children;
    for(i=0; i<col; i++) {
        if(td == NULL) { break; }
        td=td->next;
    }

    return td;
}


xmlNode *html_add_link_in_table(htmlTable *table, char *text, char *link, int col, int line){
    xmlNode *hlink;
    xmlNode *td;

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        hlink = xml_init_node(NULL, "<a>");
        xml_add_attribute(hlink, "href", link);
        hlink->text=STRDUP(text);
        xml_add_child(td, hlink);
    }

    return hlink;
}


xmlNode *html_add_link_in_node(xmlNode *node, char *text, char *link){
    xmlNode *hlink;

    hlink = xml_init_node(NULL, "<a>");
    xml_add_attribute(hlink, "href", link);
    hlink->text=STRDUP(text);
    xml_add_child(node, hlink);

    return hlink;
}


/** format a date in human readable string */
char *html_write_date(time_t date) {

    struct tm time;
    localtime_r(&date, &time);
    char *sdate;

    sdate = malloc(sizeof(char) * 17);
    sprintf(sdate, "%02d/%02d/%04d %02d:%02d", time.tm_mday, time.tm_mon + 1, time.tm_year + 1900, time.tm_hour, time.tm_min);
    return sdate;
}


void html_add_date_in_table(htmlTable *table, time_t date, int col, int line){
    xmlNode *td;

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        td->text=html_write_date(date);
    }
}


void html_set_text_in_table(htmlTable *table, char *text, int col, int line) {
    xmlNode *td;

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        td->text=STRDUP(text);
    }
}


xmlNode *html_add_image_in_table(htmlTable *table, char *image, int col, int line){
    xmlNode *img = NULL;
    xmlNode *td;

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        img = xml_init_node(NULL, "<img>");
        xml_add_attribute(img, "src", image);
        xml_add_child(td, img);
    }

    return img;
}


xmlNode *html_add_image_with_size_in_table(htmlTable *table, char *image, int width, int height,  int col, int line){
    xmlNode *img = NULL;
    xmlNode *td;
    char attribute[100];

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        img = xml_init_node(NULL, "<img>");
        xml_add_attribute(img, "src", image);
        sprintf(attribute, "%d", width);
        xml_add_attribute(img, "width", attribute);
        sprintf(attribute, "%d", height);
        xml_add_attribute(img, "height", attribute);
        xml_add_child(td, img);
    }

    return img;
}


int html_write_to_file(htmlDocument *document, char *filename) {
    return xml_write_node_in_file(filename, document);
}
