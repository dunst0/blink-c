/**
 * @file log.h
 * @author rick
 * @date 28.08.20
 * @brief File for the Log implementation
 */

#ifndef WAITUI_LOG_H
#define WAITUI_LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief The type for log events.
 */
typedef struct waitui_log_event {
    va_list ap;
    const char *format;
    const char *file;
    int line;
    struct tm *time;
    int level;
    void *userData;
} waitui_log_event;

/**
 * @brief The logging callback function type.
 */
typedef void (*waitui_log_logging_fn)(waitui_log_event *event);

/**
 * @brief The locking callback function type.
 */
typedef void (*waitui_log_lock_fn)(bool lock, void *userData);

/**
 * @brief TODO
 */
typedef enum {
    WAITUI_LOG_TRACE,
    WAITUI_LOG_DEBUG,
    WAITUI_LOG_INFO,
    WAITUI_LOG_WARN,
    WAITUI_LOG_ERROR,
    WAITUI_LOG_FATAL,
    WAITUI_LOG_MAX,
} waitui_log_level;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define waitui_log_trace(...)                                                  \
    waitui_log_writeLog(WAITUI_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define waitui_log_debug(...)                                                  \
    waitui_log_writeLog(WAITUI_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define waitui_log_info(...)                                                   \
    waitui_log_writeLog(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define waitui_log_warn(...)                                                   \
    waitui_log_writeLog(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define waitui_log_error(...)                                                  \
    waitui_log_writeLog(WAITUI_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define waitui_log_fatal(...)                                                  \
    waitui_log_writeLog(WAITUI_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Set locking function for log.
 * @param lockFn The function to be called for lock and unlock when writing logs
 * @param userData Extra user data to pass to the locking function
 */
extern void waitui_log_set_lock(waitui_log_lock_fn lockFn, void *userData);

/**
 * @brief Set minimum log level to log on stderr.
 * @param level The minimum log level from which on log appear on stderr
 */
extern void waitui_log_setLevel(waitui_log_level level);

/**
 * @brief Disable or enable logging to stderr.
 * @param enable True to disable logging to stderr, false to enable
 */
extern void waitui_log_setQuiet(bool enable);

/**
 * @brief Add the logFn as a log callback to write logs starting at the level.
 * @param logFn The function to add as a log callback
 * @param userData Extra user data to pass to the logFn
 * @param level The level from which on this log callback is executed
 * @return
 */
extern int waitui_log_addCallback(waitui_log_logging_fn logFn, void *userData,
                                  waitui_log_level level);

/**
 * @brief Add a log callback to write logs starting at the level into the file.
 * @param file The file into which to write the log
 * @param level The level from which on this log callback is executed
 * @retval 1 Successful added the file callback
 * @retval 0 No more space to add the callback
 */
extern int waitui_log_addFile(FILE *file, waitui_log_level level);

/**
 * @brief Write the log message to log with the given parameters.
 * @param level The log level for this message
 * @param file The filename where this log is written
 * @param line The line number where this log is written
 * @param format The message to write to the log
 * @param ... Extra values to be used inside the message
 */
extern void waitui_log_writeLog(waitui_log_level level, const char *file,
                                int line, const char *format, ...);

#endif//WAITUI_LOG_H
