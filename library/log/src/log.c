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
 * @brief TODO
 */
#define MAX_CALLBACKS 32


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 */
typedef struct log_callback {
    log_log_fn logFn;
    int level;
    void *user_data;
} log_callback;

/**
 * @brief TODO
 */
typedef struct log {
    log_lock_fn lockFn;
    int level;
    bool quiet;
    log_callback callbacks[MAX_CALLBACKS];
    void *user_data;
} log;


// -----------------------------------------------------------------------------
//  Local variables
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 */
static log L;

/**
 * @brief TODO
 */
static const char *log_level_strings[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

#ifdef LOG_USE_COLOR
/**
 * @brief TODO
 */
static const char *log_level_colors[] = {
        "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m",
};
#endif


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 * @param level
 * @return
 */
static const char *log_level_string(int level) {
    if (level < LOG_TRACE || level >= LOG_MAX) { return ""; }
    return log_level_strings[level];
}

/**
 * @brief TODO
 */
static void log_lock(void) {
    if (L.lockFn) { L.lockFn(true, L.user_data); }
}

/**
 * @brief TODO
 */
static void log_unlock(void) {
    if (L.lockFn) { L.lockFn(false, L.user_data); }
}

/**
 * @brief TODO
 * @param event
 */
static void log_stdout_callback(log_event *event) {
    char buffer[16];
    buffer[strftime(buffer, sizeof(buffer), "%H:%M:%S", event->time)] = '\0';

#ifdef LOG_USE_COLOR
    fprintf(event->user_data, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buffer,
            log_level_colors[event->level], log_level_strings[event->level],
            event->file, event->line);
#else
    fprintf(event->user_data, "%s %-5s %s:%d: ", buffer,
            log_level_strings[event->level], event->file, event->line);
#endif

    vfprintf(event->user_data, event->format, event->ap);
    fprintf(event->user_data, "\n");
    fflush(event->user_data);
}

/**
 * @brief TODO
 * @param event
 */
static void log_file_callback(log_event *event) {
    char buffer[64];
    buffer[strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", event->time)] =
            '\0';

    fprintf(event->user_data, "%s %-5s %s:%d: ", buffer,
            log_level_strings[event->level], event->file, event->line);

    vfprintf(event->user_data, event->format, event->ap);
    fprintf(event->user_data, "\n");
    fflush(event->user_data);
}

/**
 * @brief TODO
 * @param event
 * @param user_data
 */
static void log_init_event(log_event *event, void *user_data) {
    if (!event->time) {
        time_t t    = time(NULL);
        event->time = localtime(&t);
    }

    event->user_data = user_data;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

void log_set_lock(log_lock_fn lockFn, void *user_data) {
    L.lockFn    = lockFn;
    L.user_data = user_data;
}

void log_set_level(int level) { L.level = level; }

void log_set_quiet(bool enable) { L.quiet = enable; }

int log_add_callback(log_log_fn logFn, void *user_data, int level) {
    for (int i = 0; i < MAX_CALLBACKS; ++i) {
        if (!L.callbacks[i].logFn) {
            L.callbacks[i] = (log_callback){.logFn     = logFn,
                                            .user_data = user_data,
                                            .level     = level};
            return 1;
        }
    }
    return 0;
}

int log_add_file(FILE *file, int level) {
    return log_add_callback(log_file_callback, file, level);
}

void log_write_log(int level, const char *file, int line, const char *format,
                   ...) {
    log_event event = {
            .format = format,
            .file   = file,
            .line   = line,
            .level  = level,
    };

    log_lock();

    if (!L.quiet && level >= L.level) {
        log_init_event(&event, stderr);
        va_start(event.ap, format);
        log_stdout_callback(&event);
        va_end(event.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].logFn; ++i) {
        log_callback *cb = &L.callbacks[i];
        if (level >= cb->level) {
            log_init_event(&event, cb->user_data);
            va_start(event.ap, format);
            cb->logFn(&event);
            va_end(event.ap);
        }
    }

    log_unlock();
}