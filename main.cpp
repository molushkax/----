#include "stack.h"
#include "stack_funcs.h"
#include "hash.h"
#include "tests.h"

//TODO w e f d i with errors
//TODO test stack with raspechatka
//TODO validnost ukazateley
//TODO specificator !!!!!!!
//TODO dump - расширенный верификатор

/*#define SP "%lg"
stack_el_t a = 0;
printf("a = " SP, a);*/

int tests_run    = 0;
int tests_passed = 0;

int main () {
    stack_t stk1 = {};
    stk1.capacity = 5;

    stackerr_t err = stack_init (&stk1);
    if (err != STACK_OK) {
        print_custom_error (&stk1, err, __PRETTY_FUNCTION__, __LINE__);
        return 1;
    }

    printf ("Start is ok\n");
    printf ("Start capacity: %zu\n", stk1.capacity);
    printf ("Data ptr: %p\n\n", (void*)stk1.data);

    push_and_check (&stk1, 10, __PRETTY_FUNCTION__, __LINE__);
    push_and_check (&stk1, 20, __PRETTY_FUNCTION__, __LINE__);

    pop_and_check (&stk1,      __PRETTY_FUNCTION__, __LINE__);

    stack_dump (&stk1, __PRETTY_FUNCTION__, __LINE__);

    free (stk1.data - 1);
    err = stack_destroy (&stk1);
    if (err != STACK_OK) print_custom_error(&stk1, err, __PRETTY_FUNCTION__, __LINE__);

    printf ("Finish main part of program\n");

    err = run_all_tests ();
    if (err != STACK_OK) fprintf (stderr, "error with opening test file\n");

    return tests_passed == tests_run ? 0 : 1;
}

stackerr_t run_all_tests () {
    FILE* tests_file = NULL;

    const char* filename = "tests.log";

    tests_file = fopen (filename, "a+");
    if (tests_file == NULL) {
        fprintf (stderr, "Cannot open %s for writing\n", filename);
        return ERROR_WITH_OPENING_FILE;
    }

    fprintf (tests_file, "-------------- UNIT TESTS ---------------\n\n");

    test_init                         (tests_file);
    test_push_one                     (tests_file);
    test_push_many                    (tests_file);
    test_pop_basic                    (tests_file);
    test_pop_underflow                (tests_file);
    test_top_basic                    (tests_file);
    test_push_pop_many                (tests_file);
    test_verify_detects_broken_canary (tests_file);
    test_verify_detects_broken_hash   (tests_file);
    test_verify_detects_broken_data   (tests_file);

    fprintf (tests_file, "\n------------- RESULT -----------------\n");
    fprintf (tests_file, "Passed %d / %d\n", tests_passed, tests_run);

    fclose (tests_file);
    return STACK_OK;
}
