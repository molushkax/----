#ifndef TESTS_H_
    #define TESTS_H_

#include <stdio.h>

#define STACK_DUMP_TESTS(tests_file, stk) stack_dump_tests_func (tests_file, stk, __FUNCTION__, __LINE__)

void test_init                         (FILE* tests_file);
void test_push_one                     (FILE* tests_file);
void test_push_many                    (FILE* tests_file);
void test_pop_basic                    (FILE* tests_file);
void test_pop_underflow                (FILE* tests_file);
void test_top_basic                    (FILE* tests_file);
void test_push_pop_many                (FILE* tests_file);
void test_verify_detects_broken_canary (FILE* tests_file);
void test_verify_detects_broken_hash   (FILE* tests_file);
void test_verify_detects_broken_data   (FILE* tests_file);
void test_not_printing_data_elements_with_hash_error (FILE* tests_file);
void check (bool condition, const char* message, int line, FILE* tests_file);
void stack_dump_tests_func (FILE* tests_file, const stack_t* stk, const char* func, int line);

#endif
