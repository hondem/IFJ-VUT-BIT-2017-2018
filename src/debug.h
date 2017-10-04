#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifndef NDEBUG

#define LOG_VERBOSITY_DEBUG 2
#define LOG_VERBOSITY_INFO 1
#define LOG_VERBOSITY_WARNING 0
#define LOG_VERBOSITY_NONE (-1)

extern short log_verbosity;

#define _MESSAGE(verbosity, level_name, ...) if (log_verbosity >= verbosity) \
do {\
    fprintf(stderr, "%s [%s:%d:%s()]: ", level_name, __FILENAME__, __LINE__, __func__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

#define LOG_DEBUG(...) _MESSAGE(LOG_VERBOSITY_DEBUG, "DEBUG", __VA_ARGS__)

#define LOG_INFO(...) _MESSAGE(LOG_VERBOSITY_INFO, "INFO", __VA_ARGS__)

#define LOG_WARNING(...) _MESSAGE(LOG_VERBOSITY_WARNING, "WARNING", __VA_ARGS__)

#define DEBUG_CODE(code) code

#define NULL_POINTER_CHECK(pointer, return_value) DEBUG_CODE({\
        if ((pointer) == NULL) {\
            LOG_WARNING("NULL pointer `" #pointer "`.");\
            return return_value;\
        }\
    })

#define ASSERT(expression) DEBUG_CODE({\
        if (!(expression)) {\
            LOG_WARNING("Assertion failed: `" #expression "`.");\
        }\
    })

#define OVERRIDE_VERBOSITY(verbosity, code) \
    do { \
        short _verbosity = log_verbosity; \
        log_verbosity = verbosity; \
        code \
        log_verbosity = _verbosity; \
    } while (0)

#define DISABLE_LOG(code) OVERRIDE_VERBOSITY(LOG_VERBOSITY_NONE, code)

#else

#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define DEBUG_CODE(...)
#define NULL_POINTER_CHECK(...)
#define ASSERT(...)
#define OVERRIDE_VERBOSITY(verbosity, code) code
#define DISABLE_LOG(code) code
#define LOG_VERBOSITY_NONE (-1)
#define LOG_VERBOSITY_DEBUG LOG_VERBOSITY_NONE
#define LOG_VERBOSITY_INFO LOG_VERBOSITY_NONE
#define LOG_VERBOSITY_WARNING LOG_VERBOSITY_NONE

#endif
#endif //_DEBUG_H
