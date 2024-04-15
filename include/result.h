#pragma once
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define nil ((void*)0)

typedef struct {
    char* msg;
} error;

#define MAX_ERROR_LENGTH 256

// Workaround to create a global error so we don't need to allocate
// and free memory for every error we create.
//
// Passing NULL to 'format' means that the error exists but has no name.
static inline error* create_error(char* format, ...) {
    static error global_error = {.msg = NULL};
    static char global_error_msg[MAX_ERROR_LENGTH] = {0};

    if (format != NULL) {
        va_list args;
        va_start(args, format);
        vsnprintf(global_error_msg, sizeof(global_error_msg), format, args);
        va_end(args);
    }
    else {
        global_error_msg[0] = '\0';
    }

    global_error.msg = global_error_msg;

    return &global_error;
}

// The must(error) utility generator macro
#define create_must(NAME) \
static inline void must_##NAME(result_##NAME result) { \
    if (result.err != nil) { \
        if (strlen(result.err->msg) > 0) { \
            fprintf(stderr, "Error: %s\n", result.err->msg); \
        } \
        exit(69); \
    } \
}

// Macro to define multiple types of errors
#define define_result(TYPE, STRUCT_NAME) \
typedef struct { \
    error* err; \
    TYPE val; \
} result_##STRUCT_NAME; \
create_must(STRUCT_NAME)

// Special void result with no value
typedef struct {
    error* err;
} result_void;
create_must(void)

define_result(int, int)
define_result(char*, str)
define_result(float, float)
define_result(double, double)

// Utility to convert c results (integers) to a result_void with a given
// error message (if c_result != 0)
static inline result_void as_result(int c_result, char* format, ...) {
    result_void result = {nil};
    if (c_result != 0) {
        va_list args;
        va_start(args, format);
        result.err = create_error(format, args);
        va_end(args);
    }
    return result;
}

// Utility to 'must' c results (integers) with a format string
static inline void must_c_int(int c_result, char* format, ...) {
    must_void(as_result(c_result, format));
}

// std=c11 allows us to simulate function overload with _Generic

// Panics the program if result.err != nil, showing the result.err->msg
// and exiting.
#define must(res, ...) _Generic((res), \
    result_int: must_int, \
    result_str: must_str, \
    result_float: must_float, \
    result_double: must_double, \
    result_void: must_void, \
    int: must_c_int \
)(res __VA_OPT__(,) __VA_ARGS__)

#define FIRST(A, ...) A

// The actual result<type> definition
// Note that to start using result<type> you must previously define it
// with `define_result`. Likewise, to use the must<type> function, you must
// add its definition to the must(x) Generic macro.
#define result(TYPE) result_##TYPE
