/**
 * @file log.c
 * @author rick
 * @date 28.08.20
 * @brief File for the Log implementation
 */

#include "waitui/log.h"


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

/**
 * @brief The maximum number of possible callbacks to register.
 */
#define MAX_CALLBACKS 32


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Internal type to store the log callback function with all its info.
 */
typedef struct waitui_log_callback {
    waitui_log_logging_fn logFn;
    waitui_log_level level;
    void *userData;
} waitui_log_callback;

/**
 * @brief Internal type to store the log with all its info.
 */
typedef struct waitui_log {
    waitui_log_lock_fn lockFn;
    waitui_log_level level;
    bool quiet;
    waitui_log_callback callbacks[MAX_CALLBACKS];
    void *userData;
} log;


// -----------------------------------------------------------------------------
//  Local variables
// -----------------------------------------------------------------------------

/**
 * @brief The internal state of the log lib.
 */
static struct waitui_log L;

/**
 * @brief String representations of the log levels.
 */
static const char *waitui_log_level_strings[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

#ifdef LOG_USE_COLOR
/**
 * @brief Color representations of the log levels.
 */
static const char *waitui_log_level_colors[] = {
        "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m",
};
#endif


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief Return the string representations of the log level.
 * @param level The level to return as a string.
 * @return The string representations of the log level or empty when out of bounds.
 */
static inline const char *waitui_log_levelAsString(waitui_log_level level) {
    if (level < WAITUI_LOG_TRACE || level >= WAITUI_LOG_MAX) { return ""; }
    return waitui_log_level_strings[level];
}

/**
 * @brief Return the color representations of the log level.
 * @param level The level to return as a color.
 * @return The color representations of the log level or empty when out of bounds.
 */
static inline const char *waitui_log_levelAsColor(waitui_log_level level) {
    if (level < WAITUI_LOG_TRACE || level >= WAITUI_LOG_MAX) { return ""; }
    return waitui_log_level_colors[level];
}

/**
 * @brief Calls the locking callback function to get the lock.
 */
static inline void waitui_log_lock(void) {
    if (L.lockFn) { L.lockFn(true, L.userData); }
}

/**
 * @brief Calls the locking callback function to release the lock.
 */
static inline void waitui_log_unlock(void) {
    if (L.lockFn) { L.lockFn(false, L.userData); }
}

/**
 * @brief TODO
 * @param event
 */
static void waitui_log_stdout_callback(waitui_log_event *event) {
    char buffer[16];
    buffer[strftime(buffer, sizeof(buffer), "%H:%M:%S", event->time)] = '\0';

#ifdef LOG_USE_COLOR
    fprintf(event->userData, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buffer,
            waitui_log_levelAsColor(event->level),
            waitui_log_levelAsString(event->level), event->file, event->line);
#else
    fprintf(event->userData, "%s %-5s %s:%d: ", buffer,
            waitui_log_levelAsString(event->level), event->file, event->line);
#endif

    vfprintf(event->userData, event->format, event->ap);
    fprintf(event->userData, "\n");
    fflush(event->userData);
}

/**
 * @brief TODO
 * @param event
 */
static void waitui_log_file_callback(waitui_log_event *event) {
    char buffer[64];
    buffer[strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", event->time)] =
            '\0';

    fprintf(event->userData, "%s %-5s %s:%d: ", buffer,
            waitui_log_levelAsString(event->level), event->file, event->line);

    vfprintf(event->userData, event->format, event->ap);
    fprintf(event->userData, "\n");
    fflush(event->userData);
}

/**
 * @brief TODO
 * @param event
 * @param user_data
 */
static inline void waitui_log_init_event(waitui_log_event *event,
                                         void *userData) {
    if (!event->time) {
        time_t t    = time(NULL);
        event->time = localtime(&t);
    }

    event->userData = userData;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

void waitui_log_set_lock(waitui_log_lock_fn lockFn, void *userData) {
    L.lockFn   = lockFn;
    L.userData = userData;
}

void waitui_log_setLevel(waitui_log_level level) { L.level = level; }

void waitui_log_setQuiet(bool enable) { L.quiet = enable; }

int waitui_log_addCallback(waitui_log_logging_fn logFn, void *userData,
                           waitui_log_level level) {
    for (int i = 0; i < MAX_CALLBACKS; ++i) {
        if (!L.callbacks[i].logFn) {
            L.callbacks[i] = (waitui_log_callback){.logFn    = logFn,
                                                   .userData = userData,
                                                   .level    = level};
            return 1;
        }
    }
    return 0;
}

int waitui_log_addFile(FILE *file, waitui_log_level level) {
    return waitui_log_addCallback(waitui_log_file_callback, file, level);
}

void waitui_log_writeLog(waitui_log_level level, const char *file, int line,
                         const char *format, ...) {
    waitui_log_event event = {
            .format = format,
            .file   = file,
            .line   = line,
            .level  = level,
    };

    waitui_log_lock();

    if (!L.quiet && level >= L.level) {
        waitui_log_init_event(&event, stderr);
        va_start(event.ap, format);
        waitui_log_stdout_callback(&event);
        va_end(event.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].logFn; ++i) {
        waitui_log_callback *cb = &L.callbacks[i];
        if (level >= cb->level) {
            waitui_log_init_event(&event, cb->userData);
            va_start(event.ap, format);
            cb->logFn(&event);
            va_end(event.ap);
        }
    }

    waitui_log_unlock();
}
