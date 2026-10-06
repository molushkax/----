#ifndef STACK_FUNCS_H
    #define STACK_FUNCS_H


stackerr_t   stack_init         (stack_t* stk);
stackerr_t   stack_push         (stack_t* stk, stack_elem_t value);
stackerr_t   stack_pop          (stack_t* stk);
stackerr_t   realloc_stack_up   (stack_t* stk);
stackerr_t   realloc_stack_down (stack_t* stk);
stackerr_t   stack_destroy      (stack_t* stk);
stack_elem_t stack_top          (const stack_t* stk);
void         stack_copy         (stack_t* stk_copy, stack_t* stk);

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

#endif
