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
#define TACHE_SVN "SVN_CHECKOUT"
/** @brief compilation tag */
#define TACHE_COMPILATION "COMPILATION"
/** @brief success of tests tag */
#define TACHE_TESTS "TESTS"
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

// TYPEDEF

/**
 * data for one task
 */
typedef struct yannkins_line_t_ {
    int result;
    char *name;
    char date[17];
} yannkins_line_t;

// FUNCTIONS


/**
 * @brief Write the table
 * @param lines the datas to put in the table, must end with NULL value
 */
static void write_yannkins_table(xmlNode *document, yannkins_line_t **lines){

    yannkins_line_t *line; // current line
    int i = 0; // counter
    htmlTable *table;
    char *headers[4] = { "Last result" , "Task", "Last execution date", "Console output" };
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

    table = create_html_table(4, nbLines, headers);

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

        consoleOutputPath = malloc(sizeof(char) * (strlen(line->name) + 13));
        sprintf(consoleOutputPath, "log/%s_console", line->name);
        html_add_link_in_table(table, "see", consoleOutputPath, 3, i);
        free(consoleOutputPath);

        i++;
        line = lines[i];
    }

    html_add_table(document, table);
}


/**
 * Create a struct for a line if the file passed in argument is the log file of a task
 * @param filename complete name
 * @param basename name of file without path
 * @return NULL if filename is not the name of a task log
 */
yannkins_line_t *new_entry(char *filename, char *basename){

    yannkins_line_t *entry = NULL;
    table_csv_t *log; // content of the file
    ligne_csv_t *ligne; // a line of the file
    ligne_csv_t *last; // last line of the file

    // don't take in account "." and ".."
    if( (!strcmp(basename, ".")) || (!strcmp(basename,"..")) ){
        return NULL;
    }

    fprintf(stdout, "%s - %s\n", filename, basename);

    // don't take in account the names ending with "_console"
    if(strlen(basename)>=8){
        int index; // position in filename
        index=strlen(basename)-8;
        if(!strcmp(basename+index, "_console")){
            return NULL;
        }

    }

    // reading last line of file
    log = csv_read_file(filename, ';');
	if(log==NULL){
		return NULL;
	}

    if((log->nbCol < 2) || (log->nbLig<1)){
        fprintf(stderr, "Incorrect file: %s\n%d column(s), %d line(s)\n", basename, log->nbCol, log->nbLig);
        csv_show_table(log, stderr);
        csv_destroy_table(log);
        return NULL;
    }

    ligne=log->lignes;
    last=NULL;
    while(ligne!=NULL){
        last=ligne;
        ligne=ligne->next;
    }

    if(last==NULL){
        return NULL;
    }

    entry = malloc(sizeof(yannkins_line_t));

    entry->result = 0;
    strcpy(entry->date, "NC");
    entry->name=malloc((strlen(basename)+1)*sizeof(char));
    strcpy(entry->name, basename);

    if(strlen(last->valeurs[0])>0){
        strncpy(entry->date, last->valeurs[0], 16);
        entry->date[16]='\0';
    }

    if(strcmp(last->valeurs[1], "OK")){
        entry->result = 1;
    }

    csv_destroy_table(log);

    return entry;
}


/**
 * NON UTILISE -> technique à déplacer dans analyse.c
 * Initialyze the lines for the table.
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a table of yannkins_line_t with NULL at the end.
 */
yannkins_line_t **init_lines_rep(char *yannkinsRep){

    struct dirent *lecture; // a directory entry
    DIR *rep; //directory to cross
    char *logdir; // name of directory
    char *file; // name of a file
    yannkins_line_t **lines = NULL; // allocated table of *line
    int i = 0; // counter : number of entries in lines
    int tailleAllouee = 0; // number of allocated entries in lines

    logdir = malloc(strlen(yannkinsRep)+5);
    sprintf(logdir, "%s/log", yannkinsRep);

    rep = opendir(logdir);
    while ((lecture = readdir(rep))) {
        yannkins_line_t *entry;

        file=malloc(strlen(yannkinsRep)+strlen(lecture->d_name)+6);
        sprintf(file, "%s/log/%s", yannkinsRep, lecture->d_name);
        entry = new_entry(file, lecture->d_name);
        free(file);

        // add entry
        if(entry!=NULL){
            if(i>=tailleAllouee){
                tailleAllouee+=20;
                lines = realloc(lines, tailleAllouee*sizeof(yannkins_line_t *));
            }
            lines[i]=entry;
            i++;
        }
    }
    closedir(rep);
    lines = realloc(lines, (i+1)*sizeof(yannkins_line_t *));
    lines[i]=NULL;

    free(logdir);
    return lines;
}


/**
 * Initialyze the lines for the table.
 * @param project name of the project
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a table of yannkins_line_t with NULL at the end.
 */
yannkins_line_t **init_lines(char *project, char *yannkinsRep){

    char *logdir; // name of directory
    
    yannkins_line_t **lines = NULL; // allocated table of *line
    int i = 0; // counter : number of entries in lines
    int tailleAllouee = 0; // number of allocated entries in lines

	int j;

	char *taches[3];
	taches[0]=TACHE_SVN;
	taches[1]=TACHE_COMPILATION;
	taches[2]=TACHE_TESTS;
	
	logdir = malloc(strlen(yannkinsRep)+5);
    sprintf(logdir, "%s/log", yannkinsRep);

	for(j=0; j<3; j++){
		yannkins_line_t *entry;
		char *file; // name of a file
		char *basename;
		
		basename=malloc(strlen(taches[j])+strlen(project)+2);
		sprintf(basename, "%s_%s", taches[j], project);
		file=malloc(strlen(logdir)+strlen(basename)+2);
		sprintf(file, "%s/%s", logdir, basename);

		entry = new_entry(file, basename);
		free(basename);
        free(file);

        // add the entry
        if(entry!=NULL){
            if(i>=tailleAllouee){
                tailleAllouee+=20;
                lines = realloc(lines, tailleAllouee*sizeof(yannkins_line_t *));
            }
            lines[i]=entry;
            i++;
        }
	}

    free(logdir);
    lines = realloc(lines, (i+1)*sizeof(yannkins_line_t *));
    lines[i]=NULL;
    return lines;
}


/**
 * @brief concatenate two directory adding a '/' between the two.
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
 * Write the HTML report page of a project.
 * @param project the project definition
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a error code. Can be ERR_OPEN_FILE if an error occured while opening the file with write flag.
 */
static int write_yannkins_html(yk_project *project, char *yannkinsRep){

	yannkins_line_t **lines = init_lines(project->project_name, yannkinsRep);

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

    bandeau = read_xml_file("www/bandeau.html");
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
		elementsCherches[0]="#";
		elementsCherches[1]="author";
		elementsCherches[2]="date";
		elementsCherches[3]="commentaries";
		data_s=csv_select_columns(data, elementsCherches, 4, &nb);
		csv_truncate_column(data_s, elementsCherches[2], 20);

        html_add_title_with_hr(page, 2, "Last revisions");

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

    bandeau = read_xml_file("www/bandeau.html");
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
