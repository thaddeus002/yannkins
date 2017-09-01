/**
 * \file tache.c
 * \brief This script is a task to be executed by yannkins.
 *
 * You need put in parameters the name of the task and the command
 * One line will be added in the log file ${LOGDIR}/${TASK} :
 *     ${date};FAIL (in case of failure)
 *     ${date};OK   (in case of success)
 * The file ${LOGDIR}/${TASK}_console will content the last console output
 */

#define IC "Yannkins"
#ifndef YANNKINS_HOME
#define YANNKINS_HOME "."
#endif
#define DIR YANNKINS_HOME
#define LOGDIR DIR"/log"

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h> //system()


void usage(char *prog) {
    fprintf(stderr, "Execute a %s task\n", IC);
    fprintf(stderr, "Usage : %s <TASK_NAME> <COMMAND>\n", prog);
    exit(1);
}

char stringDate[50];

char *printDate(time_t date) {
    // TODO +%d/%m/%Y %H:%M
    sprintf(stringDate, "%d", date);
    return stringDate;
}

int main(int argc, char **argv) {

    char *tache, *commande;
    char command[200];
    time_t date;
    int resultat = 0;
    char *stringResult;
    int err = 0;
    char ficlog[200];
    FILE *flog;
    char ficconsole[200];
    FILE *fconsole;


    if(argc != 3) {
        usage(argv[0]);
    }

    tache = argv[1];
    commande = argv[2];
    date = time(NULL);

    err = mkdir(LOGDIR, 0750);
    if(err) {
        fprintf(stderr, "Could not create the directory %s\n", LOGDIR);
        fprintf(stderr, "%s : task %s not completed\n", printDate(date), tache);
        exit(err);
    }

    sprintf(ficlog, "%s/%s", LOGDIR, tache);
    flog = fopen(ficlog, "a");
    if(flog == NULL) {
        fprintf(stderr, "Could not create or modify the file %s\n", ficlog);
        fprintf(stderr, "%s : task %s not completed\n", printDate(date), tache);
        exit(1);
    }

    fseek(flog, 0, SEEK_END);
    if(!ftell(flog)) {
        fprintf(flog, "date;result\n");
    }

    sprintf(ficconsole, "%s/%s_console", LOGDIR, tache);
    fconsole = fopen(ficconsole, "w");
    if(fconsole == NULL) {
        fprintf(stderr, "Could not create or modify the file %s\n", ficconsole);
        fprintf(stderr, "%s : task %s not completed\n", printDate(date), tache);
        exit(1);
    }
    fclose(fconsole);

    sprintf(command, "%s &> %s", commande, ficconsole);
    resultat = system(command);

    if(!resultat) {
        stringResult = "OK";
    } else {
        stringResult = "FAIL";
    }

    fprintf(flog, "%s;%s\n", printDate(date), stringResult);

    fclose(flog);
}
