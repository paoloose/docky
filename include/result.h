#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "debug.h"

#define nil ((void*)0)

typedef struct {
    char* msg;
} error;

#define MAX_ERROR_LENGTH 256

static error global_error = {.msg = NULL};
static char global_error_msg[MAX_ERROR_LENGTH] = {0};

// Workaround to create a global error so we don't need to allocate
// and free memory for every error we create.
//
// Passing NULL to 'format' means that the error exists but has no name.
static inline error* create_error(char* format, ...) {
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

// Varadic version of create_error
static inline error* vcreate_error(char* format, va_list args) {
    if (format != NULL) {
        vsnprintf(global_error_msg, sizeof(global_error_msg), format, args);
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
            DOCKY_DEBUG("must() failed: %s", result.err->msg); \
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

#define return_if_err(RESULT) \
    if (RESULT.err != nil) return RESULT;

#define return_err(TYPE, ...) \
    result(TYPE) r = {0}; \
    r.err = create_error(__VA_ARGS__); \
    return r;

#define return_ok \
    result(void) r = {0}; \
    return r;

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

// Varadic version of as_result
static inline result_void vas_result(int c_result, char* format, va_list args) {
    result_void result = {nil};
    if (c_result != 0) {
        result.err = vcreate_error(format, args);
    }
    return result;
}

// Utility to 'must' c results (integers) with a format string
static inline void must_c_int(int c_result, char* format, ...) {
    va_list args;
    va_start(args, format);
    must_void(vas_result(c_result, format, args));
    va_end(args);
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

// The actual result<type> definition
// Note that to start using result<type> you must previously define it
// with `define_result`. Likewise, to use the must<type> function, you must
// add its definition to the must(x) Generic macro.
#define result(TYPE) result_##TYPE

// Utility functions to use with must()

static inline int be_legit(void* pointer) {
    if (pointer != nil) return 0;
    return 1;
}

static inline int be_non_minus_one(int value) {
    if (value != -1) return 0;
    return 1;
}

static inline int not_return(int call) {
    (void)call;
    return 1;
}

static inline int be_equal(int call, int val) {
    if (call != val) return 1;
    return 0;
}

static inline int be_non_zero(int value) {
    if (value != 0) return 1;
    return 0;
}

// Created to be chain like so failed_to(be_equal(...))
static inline bool failed_to(int to_evaluate) {
    if (to_evaluate != 0) return true;
    return false;
}
