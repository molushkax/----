#ifndef STACK_H
    #define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

typedef double stack_elem_t;

const stack_elem_t POIZON = 67*67;
const double ALMOST_ZERO_VALUE = 1e-9;

const stack_elem_t STACK_CANARY_VALUE1 = (stack_elem_t)0xBEDABE;
const stack_elem_t STACK_CANARY_VALUE2 = (stack_elem_t)0xBEDABED;
const unsigned long long STRUCT_CANARY_VALUE1 = 0xBEDABEDA;
const unsigned long long STRUCT_CANARY_VALUE2 = 0xBEDABEDAA;

#define RED "\033[31m"
#define NO_COLOR "\033[0m"

enum stackerr_t {
    STACK_OK = 0,
    ERROR_WITH_MEMORY_ALLOCATION,
    ERROR_WITH_MEMORY_REALLOCATION,
    ERROR_LEFT_STRUCT_CANARY,
    ERROR_RIGHT_STRUCT_CANARY,
    ERROR_LEFT_DATA_CANARY,
    ERROR_RIGHT_DATA_CANARY,
    ERROR_SIZE_LESS_THAN_ZERO,
    ERROR_CAPACITY_ZERO,
    ERROR_SIZE_EXCEEDS_CAPACITY,
    ERROR_DATA_IS_NULL,
    ERROR
};


struct error_map {
    int         error_code;
    const char* error_message;
};

const struct error_map errors[] = {
    { ERROR_WITH_MEMORY_ALLOCATION,   "memory allocation failed" },
    { ERROR_WITH_MEMORY_REALLOCATION, "memory reallocation failed" },
    { ERROR_LEFT_STRUCT_CANARY,       "left struct canary damaged" },
    { ERROR_RIGHT_STRUCT_CANARY,      "right struct canary damaged" },
    { ERROR_LEFT_DATA_CANARY,         "left data canary damaged" },
    { ERROR_RIGHT_DATA_CANARY,        "right data canary damaged" },
    { ERROR_SIZE_LESS_THAN_ZERO,      "stack size is less than zero" },
    { ERROR_CAPACITY_ZERO,            "stack capacity is zero" },
    { ERROR_SIZE_EXCEEDS_CAPACITY,    "size exceeds capacity" },
    { ERROR_DATA_IS_NULL,             "data pointer is NULL" },
    { ERROR,                          "generic error" },
};

struct stack_t {
    unsigned long long canary_left;
    stack_elem_t* data;
    size_t size;
    size_t capacity;
    unsigned long long canary_right;
};

stackerr_t stack_init (stack_t* stk);
stackerr_t stack_push (stack_t* stk, stack_elem_t value);
stackerr_t stack_pop  (stack_t* stk);
stackerr_t realloc_stack (stack_t* stk);

bool stack_verify (const stack_t* stk, const char* func, int line);
void stack_dump (const stack_t* stk, const char* func, int line);
void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size);
void print_custom_error (int error_code, const char* function_name, int line);
void print_error_context (const stack_t* stk, const char* func, int line);
void print_canary_error (const char* which, unsigned long long expected,
                         unsigned long long got, const char* func, int line);

void push_and_check (stack_t* stk, stack_elem_t value, const char* func, int line);
void check_of_pushing (const stack_t* stk, int err, const char* func, int line);

bool equal_double (double n1, double n2);
void poizon_memset (stack_elem_t* data, size_t from, size_t to);

#define STACK_ASSERT(stk) \
    do { \
        if (!stack_verify ((stk), __FUNCTION__, __LINE__)) { \
            fprintf (stderr, "Assert failed in %s, line %d\n", __FUNCTION__, __LINE__); \
        } \
    } while (0)

#define STACK_ASSERT_DBG(stk) STACK_ASSERT (stk) // TODO debag regim

#endif
