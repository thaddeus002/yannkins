/**
 * \file tache.c
 * \brief Execute a task and log the output and the result in
 * Yannkins' format.
 *
 * You need put in parameters the name of the task and the command.
 * One line will be added in the log file ${LOGDIR}/${TASK} :
 *     ${date};FAIL (in case of failure)
 *     ${date};OK   (in case of success)
 * The file ${LOGDIR}/${TASK}_console will content the last console
 * output.
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
#include <unistd.h>
#include <errno.h>
#include "logger.h"

void usage(char *prog) {
    fprintf(stderr, "Execute a %s task\n", IC);
    fprintf(stderr, "Usage : %s <TASK_NAME> <COMMAND>\n", prog);
    exit(1);
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
    struct stat buf;

    if(argc != 3) {
        usage(argv[0]);
    }

    tache = argv[1];
    commande = argv[2];
    date = time(NULL);

    init_log(LOG_LEVEL_INFO);

    err = mkdir(LOGDIR, 0750);
    if(err == -1) {

        if(errno != EEXIST) {
            log_error("Task %s could not create the directory %s. Exiting", tache, LOGDIR);
            exit(errno);
        }

        err = stat(LOGDIR, &buf);
        if(err) {
            log_error("Task %s : Stat failed for file %s. Exiting", tache, LOGDIR);
            exit(err);
        }

        if(!S_ISDIR(buf.st_mode)) {
            log_error("Task %s : file %s exist but is not a directory. Exiting", tache, LOGDIR);
            exit(err);
        }
    }

    sprintf(ficlog, "%s/%s", LOGDIR, tache);
    flog = fopen(ficlog, "a");
    if(flog == NULL) {
        log_error("Task %s could not create or modify the file %s. Exiting", tache, ficlog);
        exit(1);
    }

    fseek(flog, 0, SEEK_END);
    if(!ftell(flog)) {
        fprintf(flog, "date;result\n");
    }

    sprintf(ficconsole, "%s/%s_console", LOGDIR, tache);
    fconsole = fopen(ficconsole, "w");
    if(fconsole == NULL) {
        log_error("Task %s could not create or modify the file %s. Exiting", tache, ficconsole);
        exit(1);
    }
    fclose(fconsole);

    sprintf(command, "%s > %s 2>&1", commande, ficconsole);
    resultat = system(command);

    if(!resultat) {
        stringResult = "OK";
    } else {
        stringResult = "FAIL";
    }

    fprintf(flog, "%s;%s\n", printDate(date), stringResult);

    fclose(flog);
    close_log();
    return 0;
}
