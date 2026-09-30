#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

typedef double stack_elem_t;

#define STACK_ASSERT(stk) assert(stack_verify(stk, __FUNCTION__, __LINE__))

#define NDEBUG

#ifndef NDEBUG
    #define STACK_ASSERT_DBG(stk) assert(stack_verify(stk, __FUNCTION__, __LINE__)) //TODO on_dbg
#else
    #define STACK_ASSERT_DBG(stk)
#endif

#define stel lg //TODO
#define ALMOST_ZERO_VALUE 0.0000001
#define POIZON 67*67

#define NO_COLOR "\033[0m"
#define RED      "\033[1;31m"

struct stack_t {
    stack_elem_t* data;
    size_t        size;
    size_t    capacity;
};

// возвращаем код ошибки
// ашник
// verify как ассерт падает
//TODO destroy struct
struct error_map {
    int         error_code;
    const char* error_message;
};


enum stackerr_t {STACK_OK = 0,
                 ERROR_WITH_MEMORY_ALLOCATION, //TODO warning error fatal i td w e f i d
                 ERROR_FILE_NOT_FOUND,// stack empty stack oom
                 ERROR_WITH_MEMORY_REALLOCATION,
                 ERROR};

const struct error_map errors[] = {
    {STACK_OK, "You are a happy owner of stack"},
    {ERROR_WITH_MEMORY_ALLOCATION, "Allocation wasn't carried out successfully"},
    {ERROR_WITH_MEMORY_REALLOCATION, "Reallocation wasn't carried out successfully"},
    {ERROR_FILE_NOT_FOUND, "File not found"},
    {ERROR,    "Some kind of error"}
};

stackerr_t stack_init (stack_t* stk);
stackerr_t stack_push (stack_t* stk, stack_elem_t value);
void push_and_check (stack_t* stk, stack_elem_t value, const char* func, int line);
stackerr_t stack_pop  (stack_t* stk);
bool equal_double (double n1, double n2);
void check_of_pushing (const stack_t* stk, int err, const char* func, int line);
void poizon_memset (stack_elem_t* data, size_t from, size_t to);
bool stack_verify (const stack_t* stk, const char* func, int line);
stackerr_t realloc_stack (stack_t* stk);
void stack_dump (const stack_t* stk);
void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size);
void print_custom_error (int error_code, const char* function_name, int line);
