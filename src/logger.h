/**
 * \file logger.h
 * \brief log manager
 *
 * The logs will be written in the file in /var/log/yannkins/yannkins.log
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__


/** the log level */
typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    NONE
} LogLevel_t;


/**
 * Open the logs stream.
 *
 * \param minLevel the minimun level for messages to be logged
 * \return 0 in case of success or an error code
 */
int init_log(const LogLevel_t minLevel);

/**
 * Close the log stream.
 */
int close_log();

/**
 * Write a log message with a given level.
 * \param level the level of this log
 * \param message_fmt message format, followed by a variable number of parameters
 */
void log_message(const LogLevel_t level, const char *message_fmt, ...);

/**
 * Write a debug log.
 */
void log_debug(const char *message_fmt, ...);

/**
 * Write a info log.
 */
void log_info(const char *message_fmt, ...);

/**
 * Write a warning log.
 */
void log_warning(const char *message_fmt, ...);

/**
 * Write a error log.
 */
void log_error(const char *message_fmt, ...);

#endif
