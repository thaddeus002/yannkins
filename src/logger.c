/**
 * \file logger.c
 * \brief log manager
 *
 * The logs will be written in the file in /var/log/yannkins/yannkins.log
 */


#include "logger.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/** file where appends the logs */
#define LOGFILE "yannkins.log"

/** minimun level for logs to be stored */
static LogLevel_t initLevel = LOG_LEVEL_DEBUG;

/** no logs if false */
static bool initialized = false;

/**
 * Open the logs stream.
 *
 * \param minLevel the minimun level for messages to be logged
 * \return 0 in case of success or an error code
 */
int init_log(const LogLevel_t minLevel) {

    // try to create an empty logfile
    FILE *fd = fopen(LOGFILE, "a+");
    if(fd == NULL) {
        fprintf(stderr, "Can't not open or create %s\n", LOGFILE);
        return 1;
    }

    if(fclose(fd) != 0) {
        fprintf(stderr, "Fail closing file %s\n", LOGFILE);
        return 2;
    }

    initLevel = minLevel;
    initialized = true;
    return 0;
}

/**
 * Close the log stream.
 */
int close_log() {
    initialized = false;
    return 0;
}


static char *write_date() {

    static char date[20];

    time_t now;
    time(&now);

    struct tm *tm = localtime(&now);
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);

    return date;
}


static void log_message_va(const LogLevel_t level, const char *message_fmt, va_list va){

    if(!initialized || level < initLevel) {
        return;
    }

    FILE *fd = fopen(LOGFILE, "a+");
    if(fd == NULL) {
        fprintf(stderr, "Fail opening log file\n");
        return;
    }

    char *levelName;
    switch(level) {
        case LOG_LEVEL_ERROR:
            levelName = "ERROR";
            break;
        case LOG_LEVEL_WARNING:
            levelName = "WARNING";
            break;
        case LOG_LEVEL_INFO:
            levelName = "INFO";
            break;
        case LOG_LEVEL_DEBUG:
            levelName = "DEBUG";
            break;
        default:
            levelName = "";
    }

    fprintf(fd, "%s - Yannkins %s: ", write_date(), levelName);
    vfprintf(fd, message_fmt, va);
    fprintf(fd, "\n");

    fclose(fd);
}

/**
 * Write a log message with a given level.
 * \param level the level of this log
 * \param message_fmt message format, followed by a variable number of parameters
 */
void log_message(const LogLevel_t level, const char *message_fmt, ...){
    va_list ap;
    va_start(ap, message_fmt);
    log_message_va(level, message_fmt, ap);
    va_end(ap);
}


/**
 * Write a debug log.
 */
void log_debug(const char *message_fmt, ...) {
    va_list ap;
    va_start(ap, message_fmt);
    log_message_va(LOG_LEVEL_DEBUG, message_fmt, ap);
    va_end(ap);
}


/**
 * Write a info log.
 */
void log_info(const char *message_fmt, ...) {
    va_list ap;
    va_start(ap, message_fmt);
    log_message_va(LOG_LEVEL_INFO, message_fmt, ap);
    va_end(ap);
}


/**
 * Write a warning log.
 */
void log_warning(const char *message_fmt, ...) {
    va_list ap;
    va_start(ap, message_fmt);
    log_message_va(LOG_LEVEL_WARNING, message_fmt, ap);
    va_end(ap);
}


/**
 * Write a error log.
 */
void log_error(const char *message_fmt, ...) {
    va_list ap;
    va_start(ap, message_fmt);
    log_message_va(LOG_LEVEL_ERROR, message_fmt, ap);
    va_end(ap);
}
