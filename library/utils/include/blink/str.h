//
// Created by rick on 19.02.20.
//

#ifndef BLINK_STR_H
#define BLINK_STR_H

#include <stdlib.h>

typedef struct str {
    char *s;
    unsigned long int len;
} str;

#define STR_NULL \
    { NULL, 0 }

#define STR_STATIC_INIT(s) \
    { (s), sizeof((s)) - 1 }

#define STR_FMT(_pstr_)                                \
    ((_pstr_ != (str *) 0) ? (int) (_pstr_)->len : 0), \
            ((_pstr_ != (str *) 0) ? (_pstr_)->s : "")

#define STR_STATIC_SET(_pstr_, str)            \
    do {                                       \
        if ((_pstr_)) {                        \
            (_pstr_)->s = (str);               \
            (_pstr_)->len = sizeof((str)) - 1; \
        }                                      \
    } while (0)

#endif// BLINK_STR_H
