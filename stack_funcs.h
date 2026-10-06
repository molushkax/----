#ifndef STACK_FUNCS_H
#define STACK_FUNCS_H

#define STACK_VERIFY(stk) stack_verify_func (stk, __FUNCTION__, __LINE__)
#define PRINT_ERROR_CONTEXT(stk) print_error_context_func (stk, __FUNCTION__, __LINE__)
#define STACK_DUMP(stk) stack_dump_func (stk, __FUNCTION__, __LINE__)
#define PUSH_AND_CHECK(stk, value) push_and_check_func (stk, value, __FUNCTION__, __LINE__)
#define POP_AND_CHECK(stk) pop_and_check_func (stk, __FUNCTION__, __LINE__)
#define CHECK_OF_PUSHING(stk, err) check_of_pushing_func (stk, err, __FUNCTION__, __LINE__)
#define PRINT_POISON_ERROR(difference) print_poison_error_func  (difference, __FUNCTION__, __LINE__)
#define PRINT_CUSTOM_ERROR(stk, error_code) print_custom_error_func  (stk, error_code, __FUNCTION__, __LINE__)
#define PRINT_CANARY_ERROR(which, expected, got) print_canary_error_func  (which, expected, got, __FUNCTION__, __LINE__)

stackerr_t   stack_init         (stack_t* stk);
stackerr_t   stack_push         (stack_t* stk, stack_elem_t value);
stackerr_t   stack_pop          (stack_t* stk);
stackerr_t   realloc_stack_up   (stack_t* stk);
stackerr_t   realloc_stack_down (stack_t* stk);
stackerr_t   stack_destroy      (stack_t* stk);
stack_elem_t stack_top          (const stack_t* stk);
void         stack_copy         (stack_t* stk_copy, stack_t* stk);

int  stack_verify_func        (const stack_t* stk, const char* func, int line); //TODO макрос который вызывает stack_verify_f и подставляет функцию и линию
void stack_dump_func          (const stack_t* stk, const char* func, int line);
void print_data_elements      (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size);
void print_custom_error_func  (const stack_t* stk, int error_code, const char* function_name, int line);
void print_error_context_func (const stack_t* stk, const char* func, int line);
void push_and_check_func      (stack_t* stk, stack_elem_t value, const char* func, int line);
void pop_and_check_func       (stack_t* stk, const char* func, int line);
void check_of_pushing_func    (const stack_t* stk, int err, const char* func, int line);
void poison_memset            (stack_elem_t* data, size_t from, size_t to);
int  poison_check             (const stack_t* stk);
void print_poison_error_func  (int difference, const char* func, int line);

stackerr_t check_first_data_canary    (stack_elem_t canary);
stackerr_t check_second_data_canary   (stack_elem_t canary);
stackerr_t check_first_struct_canary  (unsigned long long canary);
stackerr_t check_second_struct_canary (unsigned long long canary);
void       print_canary_error_func    (const char* which, unsigned long long expected,
                                       unsigned long long got, const char* func, int line);

#endif
