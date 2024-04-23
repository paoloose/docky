#pragma once

#define DOCKY_RESET   "\x1b[0m"
#define DOCKY_RED     "\x1b[31m"
#define DOCKY_MAGENTA "\x1b[35m"
#define DOCKY_BLUE    "\x1b[34m"
#define DOCKY_CYAN    "\x1b[36m"
#define DOCKY_BRIGHT_RED   "\x1b[91m"
#define DOCKY_BRIGHT_GREEN "\x1b[92m"

// Returns the filename only
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// Environment
#ifndef DOCKY_IS_DEBUG
    #define DOCKY_IS_DEBUG 1
#endif

#ifdef DOCKY_IS_DEBUG
#   define DOCKY_DEBUG(x, ...) \
        printf( \
            DOCKY_CYAN "%s:" DOCKY_BLUE "%d(" DOCKY_CYAN "%s" DOCKY_BLUE "): " DOCKY_RESET x "\n", \
            __FILENAME__, \
            __LINE__, \
            __func__, \
            ##__VA_ARGS__ \
        );
#else
#   define DOCKY_DEBUG(x, ...) ((void)0)
#endif
