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
#include "../csv/utils.h"

/**
 * @brief old header of HTML page
 * @deprecated use HTML5_HEADER instead
 */
#define HTML4_HEADER "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">"
/** @brief Header to use in pages */
#define HTML5_HEADER "<!DOCTYPE html>"



htmlDocument *create_html_document(char *title){

    htmlDocument *doc = init_xmlNode(HTML5_HEADER, "<html>");
    xmlNode *head = init_xmlNode(NULL, "<head>");
    xmlNode *body = init_xmlNode(NULL, "<body>");
    xmlNode *htitle = init_xmlNode(NULL, "<title>");
    xmlNode *meta = init_xmlNode(NULL, "<meta>");

    htitle->text = copyString(title);
    addAttribute(meta, "charset", "utf-8");

    addChild(doc, head);
    addChild(head, htitle);
    addChild(head, meta);
    addChild(doc, body);

    return doc;
}


void destroy_html_document(htmlDocument *document) {
    destroy_xmlNode(document);
}


void html_add_css(htmlDocument *document, char *cssFile) {

    xmlNode *stylesheet = init_xmlNode(NULL, "<link>");
    xmlNode *htmlChild;

    addAttribute(stylesheet, "rel", "stylesheet");
    addAttribute(stylesheet, "href", cssFile);

    htmlChild=document->children;

    while(htmlChild!=NULL) {
        if(!strcmp(htmlChild->name, "head")) {
            addChild(htmlChild, stylesheet);
            break;
        }
        htmlChild = htmlChild->next;
    }
}


void html_add_data(htmlDocument *document, xmlNode *data){

    xmlNode *htmlChild = document->children;

    while(htmlChild!=NULL) {
        if(!strcmp(htmlChild->name, "body")) {
            addChild(htmlChild, data);
            break;
        }
        htmlChild = htmlChild->next;
    }
}


htmlList *html_add_list(htmlDocument *document) {
    htmlList *list = init_xmlNode(NULL, "<ul>");

    html_add_data(document, list);
    return list;
}


xmlNode *html_add_list_item(htmlList *list, char *item){
    xmlNode *hitem = init_xmlNode(NULL, "<li>");
    hitem->text = copyString(item);
    addChild(list, hitem);
    return hitem;
}


htmlTable *create_html_table(int nbCol, int nbLines, char **headers){
    xmlNode *table = init_xmlNode(NULL, "<table>");
    int i, j;
    xmlNode *thead = init_xmlNode(NULL, "<thead>");
    xmlNode *htr = init_xmlNode(NULL, "<tr>");
    xmlNode *tbody = init_xmlNode(NULL, "<tbody>");

    addChild(table, thead);
    addChild(thead, htr);
    addChild(table, tbody);


    for(j=0; j<nbCol; j++) {
        xmlNode *th;
        if(headers[j]==NULL) { break; }
        th = init_xmlNode(NULL, "<th>");
        th->text = copyString(headers[j]);
        addChild(htr, th);
    }

    for(i=1; i<=nbLines; i++) {
        xmlNode *tr = init_xmlNode(NULL, "<tr>");
        addChild(tbody, tr);
        for(j=1; j<=nbCol; j++) {
            xmlNode *td = init_xmlNode(NULL, "<td>");
            addChild(tr, td);
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

    table = create_html_table(nbCol, nbLines, data->entetes);

    tbody = table->children;
    while( (tbody!=NULL) && (strcmp(tbody->name,"tbody")) ) {
        tbody = tbody->next;
    }

    if(tbody == NULL) {
        // this may not append
        fprintf(stderr, "Warning: Unexpected null pointer in function html_add_table_from_data()\n");
        destroy_xmlNode(table);
        return NULL;
    }

    tr=tbody->children;
    csvLine=data->lignes;
    for(i=1; i<=nbLines; i++) {
        td=tr->children;
        for(j=1; j<=nbCol; j++) {
            td->text = copyString(csvLine->valeurs[j-1]);
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

    htitle = init_xmlNode(NULL, htag);
    htitle->text = copyString(title);

    html_add_data(document, htitle);

    return htitle;
}


xmlNode *html_add_title_with_hr(htmlDocument *document, int level, char *title) {
    xmlNode *hr = init_xmlNode(NULL, "<hr>");

    xmlNode *htitle = html_add_title(document, level, title);
    html_add_data(document, hr);

    return htitle;
}


xmlNode *html_add_link(htmlDocument *document, char *text, char *link){
    xmlNode *hlink = init_xmlNode(NULL, "<a>");

    addAttribute(hlink, "href", link);
    hlink->text=copyString(text);
    html_add_data(document, hlink);

    return hlink;
}


xmlNode *html_add_image(htmlDocument *document, char *image) {
    xmlNode *img = init_xmlNode(NULL, "<img>");

    addAttribute(img, "src", image);
    html_add_data(document, img);
    return img;
}


xmlNode *html_add_image_in_node(htmlElement *element, char *image){
    xmlNode *img = init_xmlNode(NULL, "<img>");

    addAttribute(img, "src", image);
    addChild(element, img);
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
        hlink = init_xmlNode(NULL, "<a>");
        addAttribute(hlink, "href", link);
        hlink->text=copyString(text);
        addChild(td, hlink);
    }

    return hlink;
}


xmlNode *html_add_link_in_node(xmlNode *node, char *text, char *link){
    xmlNode *hlink;

    hlink = init_xmlNode(NULL, "<a>");
    addAttribute(hlink, "href", link);
    hlink->text=copyString(text);
    addChild(node, hlink);

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
        td->text=copyString(text);
    }
}


xmlNode *html_add_image_in_table(htmlTable *table, char *image, int col, int line){
    xmlNode *img = NULL;
    xmlNode *td;

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        img = init_xmlNode(NULL, "<img>");
        addAttribute(img, "src", image);
        addChild(td, img);
    }

    return img;
}


xmlNode *html_add_image_with_size_in_table(htmlTable *table, char *image, int width, int height,  int col, int line){
    xmlNode *img = NULL;
    xmlNode *td;
    char attribute[100];

    td = find_table_cell(table, col, line);

    if(td != NULL) {
        img = init_xmlNode(NULL, "<img>");
        addAttribute(img, "src", image);
        sprintf(attribute, "%d", width);
        addAttribute(img, "width", attribute);
        sprintf(attribute, "%d", height);
        addAttribute(img, "height", attribute);
        addChild(td, img);
    }

    return img;
}


int html_write_to_file(htmlDocument *document, char *filename) {
    return write_xml_node_in_file(filename, document);
}
