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
#include "csv/csv.h"
#include "project.h"


/** @brief default working directory */
#define YANNKINS_DIR "/var/yannkins"

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
 * @brief the classic usage function.
 *
 * This function will prompt the usage message and quit.
 * @param prog the name of the program
 */
static void usage(char *prog){
	fprintf(stderr, "\nUsage: %s file.csv\n\n", prog);
	exit(1);
}


/**
 * @brief Write the table
 * @param lines the datas to put in the table, must end with NULL value
 */
static void write_yannkins_table(FILE *fd, yannkins_line_t **lines){

    yannkins_line_t *line; // current line
    int i = 0; // counter

    if(lines == NULL){
        return;
    }

    line = lines[0];

    // headers line
    fprintf(fd, "<table>\n\t<thead><tr><th>Last result</th><th>Task</th><th>Last execution date<th>Console output</th></tr></thead>\n<tbody>\n");

    while(line != NULL){

        char *icon;

        if(line->result) {
            icon = FAIL_ICON;
        } else {
            icon = OK_ICON;
        }

        fprintf(fd, "<tr>\n\t<td><img src=\"%s\" width=\"32\" height=\"32\"/></td>\n", icon);
        fprintf(fd, "\t<td>%s</td>\n", line->name);
        fprintf(fd, "\t<td>");
        fprintf(fd, "%s", line->date);
        fprintf(fd, "</td>\n");
        fprintf(fd, "\t<td><a href=\"log/%s_console\">see<a/></td>\n</tr>\n", line->name);

        i++;
        line = lines[i];
    }

    // end table
    fprintf(fd, "</tbody>\n</table>\n");
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
    log = lecture_fichier_csv_entier(filename, ';');
	if(log==NULL){
		return NULL;
	}

    if((log->nbCol < 2) || (log->nbLig<1)){
        fprintf(stderr, "Incorrect file: %s\n%d column(s), %d line(s)\n", basename, log->nbCol, log->nbLig);
        affiche_table(log, stderr);
        destroy_table_csv(log);
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

    destroy_table_csv(log);

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
 * @brief Add the content of a file in a stream.
 * @param fd the output stream
 * @param filename the name of file to add
 */
void include_file(FILE *fd, char *filename){

	FILE *in;
	char line[250];

	in = fopen(filename, "r");
	if(in != NULL) {

		while(fgets(line, 250, in)!=NULL){
			fprintf(fd, "%s", line);
		}
		fclose(in);
	}
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
 * @param project the name of the project
 * @param yannkinsRep the directory where Yannkins is installed
 * @return a error code. Can be ERR_OPEN_FILE if an error occured while opening the file with write flag.
 */
int write_yannkins_html(char *project, char *yannkinsRep){

	yannkins_line_t **lines = init_lines(project, yannkinsRep);

	char *fichier; // name of svn logs file
	char *wwwdir; // directory where put the html outputs
	char *filename; // name of the html file to create (without path)
	char *report; // name of the html file to create (with path)
	table_csv_t *data; // svn logs data
	table_csv_t *data_s; // filtrated svn logs
	char *elementsCherches[4];
	int nb; // number of OK columns for svn logs
	FILE *fd;

	wwwdir = concat_path(yannkinsRep, "www");
	if(wwwdir == NULL) {
		return ERR_MEMORY;
	}

	filename=malloc(sizeof(char)*(strlen(project)+6));
	sprintf(filename, "%s.html", project);
	
	report = concat_path(wwwdir, filename);
	free(wwwdir);
	free(filename);
	
	if(report == NULL) {
		return ERR_MEMORY;
	}

	fd = fopen(report, "w");
	free(report);

	if(fd == NULL){
		fprintf(stderr, "Can't create file %s : %d\n", report, errno);
        return ERR_OPEN_FILE;
	}

	html_ecrit_ouverture(fd);
	html_ecrit_entete(fd, TITLE);
	html_open_body(fd);
	include_file(fd, "www/bandeau.html");

	fprintf(fd, "<h1>Project %s</h1>\n", project);
	html_write_title_with_hr(fd, 2, "Results of last analysis");
    
    write_yannkins_table(fd, lines);
  
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
	fichier=malloc(sizeof(char)*(strlen(yannkinsRep)+strlen(SVNLOG)+strlen(project)+7));
	sprintf(fichier, "%s/log/%s_%s", yannkinsRep, SVNLOG, project);

	data=lecture_fichier_csv_entier(fichier, ';');
	if(data!=NULL) {
		elementsCherches[0]="#";
		elementsCherches[1]="author";
		elementsCherches[2]="date";
		elementsCherches[3]="commentaries";
		data_s=selectionne_colonnes(data, elementsCherches, 4, &nb);
		tronquer_colonne(data_s, elementsCherches[2], 20);

		fprintf(fd, "<h2>Last revisions</h2><hr/>\n");

		html_write_table(fd, data_s);
		destroy_table_csv(data_s);
		destroy_table_csv(data);
	}

	// end of file
	html_close_body(fd);
	html_ecrit_fermeture(fd);
	fclose(fd);

    return ERR_OK;
}


int main(int argc, char **argv){

	FILE *fd;
	int err;
	char *project;
	char *project_file;
	char projects_dir[1000];
	struct dirent *lecture; // an entry of projects' directory
    DIR *rep; //directory to cross
    char *logdir; // name of directory
    char *yannkinsDir; // working directory
    char *htmlFile; // index.html file
	
	yannkinsDir = getenv("YANNKINS_HOME");

	if(yannkinsDir==NULL) {
		fprintf(stderr, "Warning : YANNKINS_HOME environment variable not found\n");
		// use default
		yannkinsDir = YANNKINS_DIR;
	}

	htmlFile=concat_path(yannkinsDir, HTML_FILE);

	if(htmlFile==NULL){
		fprintf(stderr, "Can't allocate memory\n");
        return ERR_MEMORY;
	}

	fd = fopen(htmlFile, "w");

	if(fd == NULL){
		fprintf(stderr, "Can't create file %s : %d\n", htmlFile, errno);
        return ERR_OPEN_FILE;
	}

	free(htmlFile);

	html_ecrit_ouverture(fd);
	html_ecrit_entete(fd, TITLE);
	html_open_body(fd);

	include_file(fd, "www/bandeau.html");

	html_write_title(fd, 1, "Projects list");

	sprintf(projects_dir, "%s/projets", yannkinsDir);
    logdir = malloc(strlen(yannkinsDir)+5);
    sprintf(logdir, "%s/log", yannkinsDir);

	fprintf(fd, "<ul>\n");

    rep = opendir(projects_dir);
    while ((lecture = readdir(rep))) {

		if(lecture->d_type==DT_REG){

			char *project_def = malloc( (strlen(projects_dir)+strlen(lecture->d_name)+1) * sizeof(char) );
			yk_project *project_struct;

			sprintf(project_def, "%s/%s", projects_dir, lecture->d_name);
			project_struct = yk_read_project_file(project_def);
			free(project_def);

			project=project_struct->project_name;

		    fprintf(stdout, "Treatment of project %s.\n", project);
			write_yannkins_html(project, yannkinsDir);

			project_file=malloc(sizeof(char)*(strlen(project)+6));
			sprintf(project_file, "%s.html", project);
			fprintf(fd, "<li>"); html_write_link(fd, project, project_file);

			yk_destroy_project(project_struct);
		}
	}
	closedir(rep);

	fprintf(fd, "</ul>\n");

	// end of file
	html_close_body(fd);
	html_ecrit_fermeture(fd);
	fclose(fd);

	return err;
}
