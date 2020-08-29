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
 * @brief TODO
 */
typedef struct log_event {
    va_list ap;
    const char *format;
    const char *file;
    int line;
    struct tm *time;
    int level;
    void *user_data;
} log_event;

/**
 * @brief TODO
 */
typedef void (*log_log_fn)(log_event *event);

/**
 * @brief TODO
 */
typedef void (*log_lock_fn)(bool lock, void *user_data);

/**
 * @brief TODO
 */
enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_MAX,
};


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define log_trace(...) log_write_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_write_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_write_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_write_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_write_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_write_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 * @param lockFn
 * @param user_data
 */
extern void log_set_lock(log_lock_fn lockFn, void *user_data);

/**
 * @brief TODO
 * @param level
 */
extern void log_set_level(int level);

/**
 * @brief TODO
 * @param enable
 */
extern void log_set_quiet(bool enable);

/**
 * @brief TODO
 * @param logFn
 * @param user_data
 * @param level
 * @return
 */
extern int log_add_callback(log_log_fn logFn, void *user_data, int level);

/**
 * @brief TODO
 * @param file
 * @param level
 * @return
 */
extern int log_add_file(FILE *file, int level);

/**
 * @brief TODO
 * @param level
 * @param file
 * @param line
 * @param format
 * @param ...
 */
extern void log_write_log(int level, const char *file, int line,
                          const char *format, ...);

#endif//WAITUI_LOG_H
