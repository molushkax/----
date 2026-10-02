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

const stack_elem_t POISON = 67*67;

const stack_elem_t DATA_CANARY_VALUE1 = (stack_elem_t)0xBEDABE;
const stack_elem_t DATA_CANARY_VALUE2 = (stack_elem_t)0xBEDABED;
const unsigned long long STRUCT_CANARY_VALUE1 = 0xBEDABEDA;
const unsigned long long STRUCT_CANARY_VALUE2 = 0xBEDABEDAA;
const int COEFFICIENT = 2;
const int SHRINK_COEFFICIENT = 3;
const int MIN_CAPACITY = 5;
const int TWO_FOR_CANARIES = 2;

#define RED "\033[31m"
#define NO_COLOR "\033[0m"

enum stackerr_t {
    STACK_OK = 0,
    ERROR_WITH_MEMORY_ALLOCATION, //1
    ERROR_WITH_MEMORY_REALLOCATION, //2
    ERROR_LEFT_STRUCT_CANARY, //3
    ERROR_RIGHT_STRUCT_CANARY, //4
    ERROR_LEFT_DATA_CANARY, //5
    ERROR_RIGHT_DATA_CANARY, //6
    ERROR_SIZE_LESS_THAN_ZERO, //7
    ERROR_CAPACITY_ZERO, //8
    ERROR_SIZE_EXCEEDS_CAPACITY, //9
    ERROR_DATA_IS_NULL, //10
    ERROR_STACK_IS_NULL, //11
    STACK_UNDERFLOW,
    ERROR //13
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
    { ERROR_STACK_IS_NULL,            "stack pointer is NULL" },
    { STACK_UNDERFLOW,                "stack underflow: can't pop from empty stack" },
    { ERROR,                          "generic error" },
};

struct stack_t {
    unsigned long long canary_left;
    stack_elem_t* data;
    size_t size;
    size_t capacity;
    unsigned long long canary_right;
};

stackerr_t stack_init         (stack_t* stk);
stackerr_t stack_push         (stack_t* stk, stack_elem_t value);
stackerr_t stack_pop          (stack_t* stk);
stackerr_t realloc_stack_up   (stack_t* stk);
stackerr_t realloc_stack_down (stack_t* stk);
stackerr_t stack_destroy      (stack_t* stk);

bool stack_verify        (const stack_t* stk, const char* func, int line);
void stack_dump          (const stack_t* stk, const char* func, int line);
void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size);
void print_custom_error  (const stack_t* stk, int error_code, const char* function_name, int line);
void print_error_context (const stack_t* stk, const char* func, int line);
void push_and_check      (stack_t* stk, stack_elem_t value, const char* func, int line);
void pop_and_check       (stack_t* stk, const char* func, int line);
void check_of_pushing    (const stack_t* stk, int err, const char* func, int line);
void poison_memset       (stack_elem_t* data, size_t from, size_t to);
int  poison_check        (const stack_t* stk);
void print_poison_error  (int difference, const char* func, int line);

stackerr_t check_first_data_canary    (stack_elem_t canary);
stackerr_t check_second_data_canary   (stack_elem_t canary);
stackerr_t check_first_struct_canary  (unsigned long long canary);
stackerr_t check_second_struct_canary (unsigned long long canary);
void       print_canary_error         (const char* which, unsigned long long expected,
                                       unsigned long long got, const char* func, int line);


#ifndef NDEBUG
    #define STACK_ASSERT_DBG(stk) assert(stack_verify(stk, __FUNCTION__, __LINE__)) //TODO on_dbg
#else
    #define STACK_ASSERT_DBG(stk)
#endif

#define STACK_ASSERT(stk)                                                               \
    do {                                                                                \
        if (!stack_verify ((stk), __FUNCTION__, __LINE__)) {                            \
            fprintf (stderr, "Assert failed in %s, line %d\n", __FUNCTION__, __LINE__); \
        }                                                                               \
    } while (0)
#endif
