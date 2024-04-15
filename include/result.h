#pragma once
#include <stdlib.h>
#include <stdarg.h>

#define nil ((void*)0)

typedef struct {
    char* name;
} error;

#define MAX_ERROR_LENGTH 256

// Workaround to create a global error so we don't need to allocate
// and free memory for every error we create.
static inline error* create_error(char* format, ...) {
    static error global_error = {.name = NULL};
    static char global_error_name[MAX_ERROR_LENGTH] = {0};

    va_list args;
    va_start(args, format);
    vsnprintf(global_error_name, sizeof(global_error_name), format, args);
    va_end(args);

    global_error.name = global_error_name;

    return &global_error;
}

// The must(error) utility
#define create_must(NAME) \
static inline void must_##NAME(result_##NAME result) { \
    if (result.err != nil) { \
        fprintf(stderr, "Error: %s\n", result.err->name); \
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

// std=c11 allows us to simulate function overload with _Generic

// Panics the program if result.err != nil, showing the result.err->name
// and exiting.
#define must(x) _Generic((x), \
    result_int: must_int, \
    result_str: must_str, \
    result_float: must_float, \
    result_double: must_double, \
    result_void: must_void \
)(x)

// The actual result<type> definition
// Note that to start using result<type> you must previously define it
// with `define_result`. Likewise, to use the must<type> function, you must
// add its definition to the must(x) Generic macro.
#define result(TYPE) result_##TYPE
