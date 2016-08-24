/**
 * @file cree_page.c
 * @brief Create the HTML pages.
 *
 * This program must be run after the tasks of configurated projects. It will create the index pages
 * with the links to projects resumes, plus one page by projects.
 *
 * Each project's page will present the results of the tasks and a list of the latest commit's comments.
 */


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h> // free(), getenv()
#include <string.h> // strlen()
#include <errno.h>
#include <time.h>
#include "html/html.h"
#include "xml/xml.h"
#include "csv/csv.h"
#include "project.h"
#include "log_analyse.h"


/** @brief default working directory */
#define YANNKINS_DIR "/var/yannkins"
/** @brief where are the projects defined */
#define PROJECTS_DIR "projects"

/** @brief title of the html page */
#define TITLE "Yannkins' statistics - The best of continuous integration services"
/** @brief image for failed task */
#define FAIL_ICON "icons/fail.png"
/** @brief image for successfull task */
#define OK_ICON "icons/ok.png"
/** @brief index html page for report */
#define HTML_FILE "www/index.html"

// SUFFIXES FOR DIFFERENT TYPES OF TASK
/** @brief svn checkout tag */
#define REPOS_TASK "SVN_CHECKOUT"
/** @brief compilation tag */
#define COMPILATION_TASK "COMPILATION"
/** @brief success of tests tag */
#define TESTS_TASK "TESTS"
/** @brief svn logs tag */
#define SVNLOG "SVNLOG"
/** @brief git logs tag */
#define GITLOG "GITLOG"

// ERROR CODES
/** @brief Error code allocation */
#define ERR_MEMORY 2
/** @brief Error code opening file */
#define ERR_OPEN_FILE 1
/** @brief Error code OK */
#define ERR_OK 0

// STRING CONSTANTS
/** @brief how to display source code recovery task's name*/
#define REPOS_LABEL "Source code recovery"
/** @brief how to display compilation task's name*/
#define COMPILATION_LABEL "Project 's compilation"
/** @brief how to display tests task's name*/
#define TESTS_LABEL "Units' tests execution"


// TYPEDEF

/**
 * Data to show for one task. This is one line in a project's resume table.
 */
typedef struct yannkins_line_t_ {
    int result; /**< this is "ok" (0) or "fail" (1) */
    char *name; /**< the task's name */
    char date[17]; /**< the last execution date */
    char lastSuccessDate[17]; /**< the date of last successfull exectution */
    char *console_file; /**< the name of console output file */
} yannkins_line_t;

// FUNCTIONS


/**
 * @brief concatenate two strings adding a '/' between the two.
 * @param beg the beginning string
 * @param end the ending string
 * @return a pointer to the concatenated string. Must be freed.
 */
static char *concat_path(char *beg, char *end) {

    char *result = malloc(sizeof(char) *(strlen(beg)+strlen(end)+2));

    if(result==NULL){
        fprintf(stderr, "Allocation error\n");
        return NULL;
    }

    sprintf(result, "%s/%s", beg, end);
    return result;
}


/**
 * @brief Write the project's resume table at the end of a HTML document.
 *
 * The created table will contains a line by executed task to show the results
 * such as "success", "execution date", "last success date", ...
 * The last column will present a link to see the last console output.
 * @param document the HTML page where append the table
 * @param lines the datas to put in the table, must end with NULL value
 */
static void write_yannkins_table(xmlNode *document, yannkins_line_t **lines){

    yannkins_line_t *line; // current line
    int i = 0; // counter
    htmlTable *table;
    char *headers[5] = { "Last result" , "Task", "Last execution date", "Last success date", "Console output" };
    int nbLines;

    if(lines == NULL){
        return;
    }

    nbLines = 0;
    line = lines[0];
    while(line != NULL) {
        nbLines++;
        line = lines[nbLines];
    }

    table = create_html_table(5, nbLines, headers);

    line = lines[0];
    while(line != NULL){

        char *icon;
        char *consoleOutputPath;

        if(line->result) {
            icon = FAIL_ICON;
        } else {
            icon = OK_ICON;
        }

        html_add_image_with_size_in_table(table, icon, 32, 32, 0, i);

        html_set_text_in_table(table, line->name, 1, i);
        html_set_text_in_table(table, line->date, 2, i);
        html_set_text_in_table(table, line->lastSuccessDate, 3, i);

        consoleOutputPath = concat_path("log", line->console_file);
        html_add_link_in_table(table, "see", consoleOutputPath, 4, i);
        free(consoleOutputPath);

        i++;
        line = lines[i];
    }

    html_add_table(document, table);
}


/**
 * @brief Create a struct for a line if the file passed in argument is the log file of a task.
 * @param filename complete name
 * @param basename name of file without path. Console output is suppose to be in file "${basename}_console"
 * @param entryName the task's name, basename will be use instead if NULL
 * @return NULL if filename is not the name of a task log
 */
static yannkins_line_t *new_entry(char *filename, char *basename, char *entryName){

    yannkins_line_t *entry = NULL; // return value
    table_csv_t *log; // content of the file
    ligne_csv_t *logline; // a line of the file
    ligne_csv_t *last; // last line of the file
    char *lastSuccessDate = NULL; // record for last success date
    char *name =entryName; // task's name

    // don't take in account "." and ".."
    if( (!strcmp(basename, ".")) || (!strcmp(basename,"..")) ){
        return NULL;
    }

    // don't take in account the names ending with "_console"
    if(strlen(basename)>=8){
        int index; // position in filename
        index=strlen(basename)-8;
        if(!strcmp(basename+index, "_console")){
            return NULL;
        }
    }

    // reading log file
    log = csv_read_file(filename, ';');
    if(log==NULL){
        return NULL;
    }

    if((log->nbCol < 2) || (log->nbLig < 1)){
        fprintf(stderr, "Incorrect file: %s\n%d column(s), %d line(s)\n", basename, log->nbCol, log->nbLig);
        csv_show_table(log, stderr);
        csv_destroy_table(log);
        return NULL;
    }

    // view last log line
    logline=log->lignes;
    last=NULL;
    while(logline!=NULL){
        last=logline;
        if(!strcmp(last->valeurs[1], "OK")){
            lastSuccessDate = last->valeurs[0];
        }
        logline=logline->next;
    }

    if(last==NULL){
        return NULL;
    }

    // result
    entry = malloc(sizeof(yannkins_line_t));

    entry->result = 0;
    strcpy(entry->date, "NC");
    if(name == NULL) { name = basename; }
    entry->name=malloc((strlen(name)+1)*sizeof(char));
    strcpy(entry->name, name);
    strcpy(entry->lastSuccessDate, "-");

    if(strlen(last->valeurs[0])>0){
        strncpy(entry->date, last->valeurs[0], 16);
        entry->date[16]='\0';
    }

    if(strcmp(last->valeurs[1], "OK")){
        entry->result = 1;
    }

    if((lastSuccessDate != NULL) && (strlen(lastSuccessDate)>0)){
        strncpy(entry->lastSuccessDate, lastSuccessDate, 16);
        entry->lastSuccessDate[16]='\0';
    }

    entry->console_file = malloc((strlen(basename)+1+8)*sizeof(char));
    sprintf(entry->console_file, "%s_console", basename);

    // end
    csv_destroy_table(log);

    return entry;
}


/**
 * Initialyze the lines for the table.
 * @param project the project's definition
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a table of yannkins_line_t with NULL at the end.
 */
static yannkins_line_t **init_lines(yk_project *project, char *yannkinsRep){

    char *logdir; // name of directory

    yannkins_line_t **lines = NULL; // allocated table of *line
    int i = 0; // counter : number of entries in lines
    int allocatedSize = 0; // number of allocated entries in lines

    int j;

    char *tasks[3];
    tasks[0]=REPOS_TASK;
    tasks[1]=COMPILATION_TASK;
    tasks[2]=TESTS_TASK;

    logdir = malloc(strlen(yannkinsRep)+5);
    sprintf(logdir, "%s/log", yannkinsRep);

    for(j=0; j<3; j++){
        yannkins_line_t *entry;
        char *file; // name of a file
        char *basename;
        char *taskName = NULL;

        basename=malloc(strlen(tasks[j])+strlen(project->project_name)+2);
        sprintf(basename, "%s_%s", tasks[j], project->project_name);
        file=malloc(strlen(logdir)+strlen(basename)+2);
        sprintf(file, "%s/%s", logdir, basename);

        // TODO : do this for all task, in a better way
        switch(j) {
        case 0:
            taskName = REPOS_LABEL;
            break;
        case 1:
            taskName = COMPILATION_LABEL;
            break;
        case 2:
            taskName = TESTS_LABEL;
        }
        entry = new_entry(file, basename, taskName);
        free(basename);
        free(file);

        // add the entry
        if(entry!=NULL){
            // take in account the size for the last NULL pointer
            if(i>=allocatedSize-1){
                allocatedSize+=20;
                lines = realloc(lines, allocatedSize*sizeof(yannkins_line_t *));
            }
            lines[i]=entry;
            i++;
        }
    }

    free(logdir);
    lines[i]=NULL;
    return lines;
}


/**
 * Write the HTML report page of a project.
 * @param project the project definition
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a error code. Can be ERR_OPEN_FILE if an error occured while opening the file with write flag.
 */
static int write_yannkins_html(yk_project *project, char *yannkinsRep){

    yannkins_line_t **lines = init_lines(project, yannkinsRep);

    char *fichier = NULL; // name of svn logs file
    char *wwwdir; // directory where put the html outputs
    char *filename; // name of the html file to create (without path)
    char *report; // name of the html file to create (with path)
    table_csv_t *data; // svn logs data
    table_csv_t *data_s; // filtrated svn logs
    char *elementsCherches[4];
    int nb; // number of OK columns for svn logs
    htmlDocument *page;
    xmlNode *bandeau;
    char *content;

    page = create_html_document(TITLE);
    html_add_css(page, "style/style.css");

    bandeau = xml_read_file("www/bandeau.html");
    html_add_data(page, bandeau);

    content = malloc(sizeof(char) * (strlen(project->project_name) + 9));
    sprintf(content, "Project %s", project->project_name);
    html_add_title(page, 1, content);
    free(content);

    html_add_title_with_hr(page, 2, "Results of last analysis");

    write_yannkins_table(page, lines);

    // freeing memory
    if(lines != NULL){
        int i = 0;
        while(lines[i]!=NULL){
            if(lines[i]->name!=NULL){
                free(lines[i]->name);
            }
            if(lines[i]->console_file!=NULL) {
                free(lines[i]->console_file);
            }
            free(lines[i]);
            i++;
        }
        free(lines);
    }

    // logs' table
    if(project->versioning_type == SVN) {
        fichier=malloc(sizeof(char)*(strlen(yannkinsRep)+strlen(SVNLOG)+strlen(project->project_name)+7));
        sprintf(fichier, "%s/log/%s_%s", yannkinsRep, SVNLOG, project->project_name);
    } else if(project->versioning_type == GIT) {
        fichier=malloc(sizeof(char)*(strlen(yannkinsRep)+strlen(GITLOG)+strlen(project->project_name)+7));
        sprintf(fichier, "%s/log/%s_%s", yannkinsRep, GITLOG, project->project_name);
    }

    if(fichier != NULL) {
        data=csv_read_file(fichier, ';');
    }

    if(data!=NULL) {
        char subtitle[500];

        elementsCherches[0]="#";
        elementsCherches[1]="author";
        elementsCherches[2]="date";
        elementsCherches[3]="commentaries";
        data_s=csv_select_columns(data, elementsCherches, 4, &nb);
        csv_truncate_column(data_s, elementsCherches[2], 20);

        sprintf(subtitle, "Last %d commits (by %d authors)", data->nbLig, get_authors_number(data));
        html_add_title_with_hr(page, 2, subtitle);

        html_add_table_from_data(page, data_s);

        csv_destroy_table(data_s);
        csv_destroy_table(data);
    }

    // write file
    wwwdir = concat_path(yannkinsRep, "www");
    if(wwwdir == NULL) {
        return ERR_MEMORY;
    }

    filename=malloc(sizeof(char)*(strlen(project->project_name)+6));
    sprintf(filename, "%s.html", project->project_name);

    report = concat_path(wwwdir, filename);
    free(wwwdir);
    free(filename);

    if(report == NULL) {
        return ERR_MEMORY;
    }

    html_write_to_file(page, report);
    destroy_html_document(page);
    free(report);

    return ERR_OK;
}


int main(int argc, char **argv){

    char *project;
    char *project_file;
    char projects_dir[1000];
    struct dirent *lecture; // an entry of projects' directory
    DIR *rep; //directory to cross
    char *logdir; // name of directory
    char *yannkinsDir; // working directory
    char *htmlFile; // index.html file
    htmlDocument *page;
    xmlNode *bandeau;
    htmlList *list;
    xmlNode *listItem;


    yannkinsDir = getenv("YANNKINS_HOME");

    if(yannkinsDir==NULL) {
        fprintf(stderr, "Warning : YANNKINS_HOME environment variable not found, using \"%s\"\n", YANNKINS_DIR);
        // use default
        yannkinsDir = YANNKINS_DIR;
    }

    page = create_html_document(TITLE);
    html_add_css(page, "style/style.css");

    bandeau = xml_read_file("www/bandeau.html");
    html_add_data(page, bandeau);

    html_add_title(page, 1, "Projects list");

    sprintf(projects_dir, "%s/%s", yannkinsDir, PROJECTS_DIR);
    logdir = malloc(strlen(yannkinsDir)+5);
    sprintf(logdir, "%s/log", yannkinsDir);

    list = html_add_list(page);

    rep = opendir(projects_dir);
    if(rep == NULL) {
        fprintf(stderr, "Error: can't open directory %s\n", projects_dir);
        return 1;
    }

    while ((lecture = readdir(rep))) {

        if(lecture->d_type==DT_REG){

            char *project_def = malloc( (strlen(projects_dir)+strlen(lecture->d_name)+2) * sizeof(char) );
            yk_project *project_struct;

            sprintf(project_def, "%s/%s", projects_dir, lecture->d_name);
            project_struct = yk_read_project_file(project_def);
            free(project_def);

            project=project_struct->project_name;

            fprintf(stdout, "Treatment of project %s.\n", project_struct->project_name);
            write_yannkins_html(project_struct, yannkinsDir);

            project_file=malloc(sizeof(char)*(strlen(project)+6));
            sprintf(project_file, "%s.html", project);

            listItem = html_add_list_item(list, NULL);
            html_add_link_in_node(listItem, project, project_file);

            yk_destroy_project(project_struct);
        }
    }
    closedir(rep);


    // write file

    htmlFile=concat_path(yannkinsDir, HTML_FILE);

    if(htmlFile==NULL){
        fprintf(stderr, "Can't allocate memory\n");
        return ERR_MEMORY;
    }

    html_write_to_file(page, htmlFile);
    free(htmlFile);
    destroy_html_document(page);

    return 0;
}
