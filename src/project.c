/** @file project.c define a project */

#include "project.h"
#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h>

yk_project *yk_read_project_file(char *filename){

	FILE *fd;
	yk_project *project = NULL;
	char ligne[250]; // a line of file
	char *value; //position after the '=' in ligne
	int i; // position in string

	fd = fopen(filename, "r");

	if (fd == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", filename);
	} else {

		project = malloc(sizeof(yk_project));

		project->project_name=NULL;
		project->svn_depot=NULL;
		project->svn_user=NULL;
		project->svn_password=NULL;
		project->compil_cmd=NULL;
		project->tests_cmd=NULL;
	
		while(fgets(ligne, 250, fd)!=NULL){

			// don't care for comments lines
			if(ligne[0]=='#') {
				continue;
			}

			// we are looking for '=' position, but not at the beginning
			i=1;
			while(i<strlen(ligne)){
				if(ligne[i]=='=') {
					break;
				}
				i++;
			}

			if(i==strlen(ligne)){
				continue;
			}

			value=ligne+i+1;
			ligne[i]='\0';

			if(value[0]=='"'){
				value=value+1;
			}

			while ((value[strlen(value)-1]=='"') || (value[strlen(value)-1]=='\n') || (value[strlen(value)-1]=='\r')){
				value[strlen(value)-1]='\0';
			}

			

			if(!strcmp(ligne, "PROJECT_NAME")){
				project->project_name=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->project_name, value);
			} else if(!strcmp(ligne, "SVN_DEPOT")){
				project->svn_depot=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->svn_depot, value);
			} else if(!strcmp(ligne, "SVN_USER")){
				project->svn_user=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->svn_user, value);
			} else if(!strcmp(ligne, "SVN_PASSWD")){
				project->svn_password=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->svn_password, value);
			} else if(!strcmp(ligne, "COMPIL")){
				project->compil_cmd=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->compil_cmd, value);
			} else if(!strcmp(ligne, "TESTS_UNI")){
				project->tests_cmd=malloc((strlen(value)+1)*sizeof(char));
				strcpy(project->tests_cmd, value);
			}

			// if we have not read the end of line
			if (value[strlen(value)-1] != '\n') {
				while(fgets(ligne, 250, fd)!=NULL){
					if(ligne[strlen(ligne)-1]=='\n') {
						break;
					}
				}
			}
		}

		fclose(fd);
	}

	return project;
}

void yk_destroy_project(yk_project *project){

	if(project == NULL){
		return;
	}

	if(project->project_name != NULL){
		free(project->project_name);
	}
	if(project->svn_depot != NULL){
		free(project->svn_depot);
	}
	if(project->svn_user != NULL){
		free(project->svn_user);
	}
	if(project->svn_password != NULL){
		free(project->svn_password);
	}
	if(project->compil_cmd != NULL){
		free(project->compil_cmd);
	}
	if(project->tests_cmd != NULL){
		free(project->tests_cmd);
	}

	free(project);
}
