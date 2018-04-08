/**
 * \file project.c
 * \brief define a project
 */

#include "project.h"
#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h>

yk_project *yk_read_project_file(char *filename){

    FILE *fd; /* file descriptor */
    yk_project *project = NULL; /* the new project */
    char line[250]; // a line of file
    char *value; //position after the '=' in ligne
    int i; // position in string
    int eol; // end of line reatched

    fd = fopen(filename, "r");

    if (fd == NULL) {
        fprintf(stderr, "Can not open file %s\n", filename);
    } else {

        project = malloc(sizeof(yk_project));

        project->project_name=NULL;
        project->versioning_type=NONE;
        project->repository=NULL;
        project->repos_user=NULL;
        project->repos_password=NULL;
        project->compil_cmd=NULL;
        project->tests_cmd=NULL;

        while(fgets(line, 250, fd)!=NULL){

            eol=0;

            // don't care for comments lines
            if(line[0]=='#') {
                continue;
            }

            // we are looking for '=' position, but not at the beginning
            i=1;
            while(i<strlen(line)){
                if(line[i]=='=') {
                    break;
                }
                i++;
            }

            if(i==strlen(line)){
                continue;
            }

            value=line+i+1;
            line[i]='\0';

            if(value[0]=='"'){
                value=value+1;
            }

            while ((value[strlen(value)-1]=='"') || (value[strlen(value)-1]=='\n') || (value[strlen(value)-1]=='\r')){
                if(value[strlen(value)-1]=='\n') { eol = 1; }
                value[strlen(value)-1]='\0';
            }

            if(!strcmp(line, "PROJECT_NAME")){
                project->project_name=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->project_name, value);
            } else if(!strcmp(line, "SVN_DEPOT")){
                project->versioning_type=SVN;
                project->repository=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->repository, value);
            } else if(!strcmp(line, "GIT_DEPOT")){
                project->versioning_type=GIT;
                project->repository=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->repository, value);
            } else if(!strcmp(line, "SVN_USER") || !strcmp(line, "GIT_USER")){
                project->repos_user=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->repos_user, value);
            } else if(!strcmp(line, "SVN_PASSWD") || !strcmp(line, "GIT_PASSWD")){
                project->repos_password=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->repos_password, value);
            } else if(!strcmp(line, "COMPIL")){
                project->compil_cmd=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->compil_cmd, value);
            } else if(!strcmp(line, "TESTS_UNI")){
                project->tests_cmd=malloc((strlen(value)+1)*sizeof(char));
                strcpy(project->tests_cmd, value);
            }

            // if we have not read the end of line
            if (!eol) {
                while(fgets(line, 250, fd)!=NULL){
                    if(line[strlen(line)-1]=='\n') {
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
    if(project->repository != NULL){
        free(project->repository);
    }
    if(project->repos_user != NULL){
        free(project->repos_user);
    }
    if(project->repos_password != NULL){
        free(project->repos_password);
    }
    if(project->compil_cmd != NULL){
        free(project->compil_cmd);
    }
    if(project->tests_cmd != NULL){
        free(project->tests_cmd);
    }

    free(project);
}
