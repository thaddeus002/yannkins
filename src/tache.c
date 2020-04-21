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
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h> //system()
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "logger.h"

static void usage(char *prog) {
    fprintf(stderr, "Execute a %s task\n", IC);
    fprintf(stderr, "Usage : %s <TASK_NAME> <COMMAND>\n", prog);
    exit(1);
}

static char stringDate[50];

/**
 * Format a timestamp in a readable format (%d/%m/%Y %H:%M).
 * \date a timestamp
 * \return a char string representing the date. This is a pointer to a
 *         statically allocated memory zone : don't free it.
 */
static char *printDate(time_t date) {
    struct tm *sdate = localtime(&date);
    // %d/%m/%Y %H:%M
    sprintf(stringDate, "%02d/%02d/%04d %02d:%02d", sdate->tm_mday, sdate->tm_mon+1, sdate->tm_year+1900,
            sdate->tm_hour, sdate->tm_min);
    return stringDate;
}

/**
 * Save the task's result in the appropriate file.
 *
 * \param date the execution date
 * \param resultat task's return value (0==success)
 * \param tache the task's name
 * \return 0 if the result was saved
 */
static int save_result(time_t date, int resultat, const char *tache){

    char *stringResult;
    char *ficlog;
    FILE *flog = NULL;

    if(!resultat) {
        stringResult = "OK";
    } else {
        stringResult = "FAIL";
    }

    ficlog = malloc(sizeof(char) * (strlen(LOGDIR) + strlen(tache) + 2));
    if(ficlog == NULL) {
        log_error("Task %s could not allocate memory. Task's result won't be saved.", tache);
        return 1;
    }

    sprintf(ficlog, "%s/%s", LOGDIR, tache);
    flog = fopen(ficlog, "a");
    free(ficlog);
    if(flog == NULL) {
        log_error("Task %s could not create or modify the file %s. Task's result won't be saved.", tache, ficlog);
        return 2;
    }

    fseek(flog, 0, SEEK_END);
    if(!ftell(flog)) {
        fprintf(flog, "date;result\n");
    }

    fprintf(flog, "%s;%s\n", printDate(date), stringResult);
    fclose(flog);
    return 0;
}


/**
 * Run the "system" function to execute a command redirecting output in
 * a file.
 *
 * \param logfile the file to use for the command's output
 * \param command the command to execute in a shell
 * \return the exit status of the command
 */
static int exec_command(char *logfile, const char *command) {

    remove(logfile);

    close(1);
    if(open(logfile, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1) {
        log_error("fail to write output on %s", logfile);
    } else {
        close(2);
        dup2(1, 2);
    }

    return system(command)/256;
}


/**
 * Run the "system" function to execute a command redirecting output in
 * a file. The run is done in a forked process to not lose the standard
 * outputs.
 *
 * \param tache task's name
 * \param commande the command to execute in a shell
 * \param resultat to store the return value of the command
 * \return 0 is the run was done
 */
static int run_task(const char *tache, const char *commande, int *resultat) {

    int err;
    char *ficconsole;
    FILE *fconsole = NULL;
    pid_t pid;

    ficconsole = malloc(sizeof(char) * (strlen(LOGDIR) + strlen(tache) + 10));
    if(ficconsole == NULL) {
        log_error("Task %s could not allocate memory. Task aborted.", tache);
        return 1;
    }

    sprintf(ficconsole, "%s/%s_console", LOGDIR, tache);
    fconsole = fopen(ficconsole, "w");
    if(fconsole == NULL) {
        log_error("Task %s could not create or modify the file %s. Task aborted", tache, ficconsole);
        return 2;
    }
    fclose(fconsole);

    pid = fork();
    if(pid == -1) {
        log_error("Fork failure : command \"%s\" not runned", commande);
        return 1;
    }

    if(pid == 0) {
        int err = exec_command(ficconsole, commande);
        free(ficconsole);
        log_debug("forked process exits with status %d", err);
        exit(err);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (!WIFEXITED(wstatus)) {
            *resultat = 1;
        } else {
            *resultat = WEXITSTATUS(wstatus);
            log_debug("parent process get status %d", *resultat);
        }
    }

    free(ficconsole);
    return 0;
}


int main(int argc, char **argv) {

    char *tache;
    char *commande;
    time_t date;
    int resultat = 0;
    int err = 0;
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
            err = errno;
            goto complete;
        }

        err = stat(LOGDIR, &buf);
        if(err) {
            log_error("Task %s : Stat failed for file %s. Exiting", tache, LOGDIR);
            goto complete;
        }

        if(!S_ISDIR(buf.st_mode)) {
            log_error("Task %s : file %s exist but is not a directory. Exiting", tache, LOGDIR);
            goto complete;
        }
    }

    err = run_task(tache, commande, &resultat);
    if(err) {
        log_error("task %s was not executed", tache);
        goto complete;
    }

    err = save_result(date, resultat, tache);

  complete:
    close_log();
    return err;
}
