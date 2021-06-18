/**
 * @file list.h
 * @author rick
 * @date 19.02.20
 * @brief File for the String implementation
 */

#ifndef WAITUI_STR_H
#define WAITUI_STR_H

#include <stdlib.h>
#include <string.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type for strings that do not need to be '\0' terminated.
 */
typedef struct str {
    char *s;
    unsigned long int len;
} str;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define STR_NULL_INIT                                                          \
    { NULL, 0 }

#define STR_STATIC_INIT(_str_)                                                 \
    { (_str_), sizeof((_str_)) - 1 }

#define STR_FMT(_pstr_)                                                        \
    (((_pstr_) != (str *) 0) ? (int) (_pstr_)->len : 0),                       \
            (((_pstr_) != (str *) 0) ? (_pstr_)->s : "")

#define STR_STATIC_SET(_pstr_, _str_)                                          \
    do {                                                                       \
        if ((_pstr_)) {                                                        \
            (_pstr_)->s   = (_str_);                                           \
            (_pstr_)->len = sizeof((_str_)) - 1;                               \
        }                                                                      \
    } while (0)

#define STR_STATIC_COPY(_dstr_, _str_)                                         \
    do {                                                                       \
        if ((_dstr_)) {                                                        \
            char *tmp     = (_str_);                                           \
            (_dstr_)->len = sizeof((_str_)) - 1;                               \
            (_dstr_)->s   = calloc((_dstr_)->len, sizeof(*(_dstr_)->s));       \
            if ((_dstr_)->s) { memcpy((_dstr_)->s, tmp, (_dstr_)->len); }      \
        }                                                                      \
    } while (0)

#define STR_COPY(_dstr_, _pstr_)                                               \
    do {                                                                       \
        if ((_pstr_) && (_dstr_)) {                                            \
            (_dstr_)->len = (_pstr_)->len;                                     \
            (_dstr_)->s   = calloc((_pstr_)->len, sizeof(*(_pstr_)->s));       \
            if ((_dstr_)->s) {                                                 \
                memcpy((_dstr_)->s, (_pstr_)->s, (_dstr_)->len);               \
            }                                                                  \
        }                                                                      \
    } while (0)

#define STR_COPY_WITH_NUL(_dstr_, _pstr_)                                      \
    do {                                                                       \
        if ((_pstr_) && (_dstr_)) {                                            \
            (_dstr_)->len = (_pstr_)->len;                                     \
            (_dstr_)->s   = calloc((_pstr_)->len + 1, sizeof(*(_pstr_)->s));   \
            if ((_dstr_)->s) {                                                 \
                memcpy((_dstr_)->s, (_pstr_)->s, (_dstr_)->len);               \
            }                                                                  \
        }                                                                      \
    } while (0)

#define STR_FREE(_pstr_)                                                       \
    do {                                                                       \
        if ((_pstr_)) {                                                        \
            if ((_pstr_)->s) { free((_pstr_)->s); };                           \
            (_pstr_)->len = 0;                                                 \
            (_pstr_)->s   = NULL;                                              \
        }                                                                      \
    } while (0)

#endif//WAITUI_STR_H
