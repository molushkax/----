#include "stack.h"
#include "stack_funcs.h"
#include "hash.h"
#include "tests.h"
//------------------------ Т Е С Т Ы -------------------------
extern int tests_run;
extern int tests_passed;

// ---------------------------------------------------------------------
// 1. инициализация
// ---------------------------------------------------------------------
void test_init (FILE* tests_file) {
    fprintf (tests_file, "(1) test_init:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (stk.size == 0,     "size should be 0",     __LINE__, tests_file);
    check (stk.capacity == 5, "capacity should be 5", __LINE__, tests_file);
    check (stk.data != NULL,  "data should not be NULL", __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 2. один push
// ---------------------------------------------------------------------
void test_push_one (FILE* tests_file) {
    fprintf (tests_file, "\n\n(2) test_push_one:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    stack_push (&stk, 42);

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (stk.size == 1,           "size should be 1",  __LINE__, tests_file);
    check (stk.data[0] == 42,       "data[0] should be 42", __LINE__, tests_file);
    check (stack_top (&stk) == 42,  "top should be 42",  __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 3. много пушей (сверх capacity — сработает realloc_stack_up)
// ---------------------------------------------------------------------
void test_push_many (FILE* tests_file) {
    fprintf (tests_file, "\n\n(3) test_push_many:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    for (int i = 1; i <= 10; i++) stack_push (&stk, i);

    check (stk.size == 10,     "size should be 10", __LINE__, tests_file);
    check (stk.capacity >= 10, "capacity should grow to at least 10", __LINE__, tests_file);

    bool all_ok = true;
    for (int i = 0; i < 10; i++) {
        if (stk.data[i] != i + 1) { all_ok = false; break; }
    }

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (all_ok, "all 10 elements should keep their values", __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 4. oбычный pop
// ---------------------------------------------------------------------
void test_pop_basic (FILE* tests_file) {
    fprintf (tests_file, "\n\n(4) test_pop_basic:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    stack_push (&stk, 10);
    stack_push (&stk, 20);
    stack_push (&stk, 30);
    stack_pop  (&stk);

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (stk.size == 2,                   "size should be 2", __LINE__ , tests_file);
    check (stk.data[0] == 10,           "data[0] should be 10", __LINE__, tests_file);
    check (stk.data[1] == 20,           "data[1] should be 20", __LINE__, tests_file);
    check (stk.data[2] == POISON,   "data[2] should be POISON", __LINE__, tests_file);
    check (stack_top (&stk) == 20,          "top should be 20", __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 5. pop на пустом стеке, должен быть STACK_UNDERFLOW
// ---------------------------------------------------------------------
void test_pop_underflow (FILE* tests_file) {
    fprintf (tests_file, "\n\n(5) test_pop_underflow:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    stackerr_t err = stack_pop (&stk);

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (err == STACK_UNDERFLOW, "pop on empty should return STACK_UNDERFLOW", __LINE__, tests_file);
    check (stk.size == 0,                                  "size should stay 0", __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 6. stack_top: возвращает верх, не меняет стек
// ---------------------------------------------------------------------
void test_top_basic (FILE* tests_file) {
    fprintf (tests_file, "\n\n(6) test_top_basic:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    stack_push (&stk, 10);
    stack_push (&stk, 20);
    stack_push (&stk, 30);

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (stack_top (&stk) == 30,                 "top should be 30",  __LINE__, tests_file);
    check (stk.size == 3,          "size should not change after top",  __LINE__, tests_file);
    check (stack_top (&stk) == 30,           "top should still be 30",  __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0,           "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 7. стресс-тест: много push/pop подряд
// ---------------------------------------------------------------------
void test_push_pop_many (FILE* tests_file) {
    fprintf (tests_file, "\n\n(7) test_push_pop_many:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    bool size_ok = true;
    for (int i = 0; i < 30; i++) {
        stack_push (&stk, i);
        stack_pop  (&stk);
        if (stk.size != 0) { size_ok = false; break; }
    }

    STACK_DUMP_TESTS (tests_file,  &stk);

    check (size_ok,       "after each push+pop size should be 0", __LINE__, tests_file);
    check (stk.size == 0,          "in the end size should be 0", __LINE__, tests_file);
    check (STACK_VERIFY(&stk) == 0, "verify should be true", __LINE__, tests_file);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 8. убитая канарейка структуры
// ---------------------------------------------------------------------
void test_verify_detects_broken_canary (FILE* tests_file) {
    fprintf (tests_file, "\n\n(8) test_verify_detects_broken_canary:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    stk.canary_left = 0;   // специально ломаем

    check (STACK_VERIFY(&stk) == 2, "verify should return false when canary is broken", __LINE__, tests_file);

    STACK_DUMP_TESTS (tests_file,  &stk);

    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 9. изменение size
// ---------------------------------------------------------------------
void test_verify_detects_broken_hash (FILE* tests_file) {
    fprintf (tests_file, "\n\n(9) test_verify_detects_broken_hash:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);
    stack_push (&stk, 10);
    stack_push (&stk, 20);

    size_t saved_size = stk.size; // сохраняем для freeшки
    stk.size = 999;   // меняем size, но hash не пересчитываем


    check (STACK_VERIFY(&stk) == 3, "verify should return false when size is changed without updating hash", __LINE__, tests_file);

    STACK_DUMP_TESTS (tests_file,  &stk);

    stk.size = saved_size;
    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 10. подмена элемента data
// ---------------------------------------------------------------------
void test_verify_detects_broken_data (FILE* tests_file) {
    fprintf (tests_file, "\n\n(10) test_verify_detects_broken_data:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);
    stack_push (&stk, 10);
    stack_push (&stk, 20);
    stack_push (&stk, 30);

    stack_elem_t saved = stk.data[1];
    stk.data[1] = 999;   // подменяем элемент, hash не пересчитываем


    check (STACK_VERIFY(&stk) == 1,
           "verify should return false when data element is changed",
           __LINE__, tests_file);

    STACK_DUMP_TESTS (tests_file,  &stk);

    stk.data[1] = saved;   // возвращаем как было для норм free
    free (stk.data - 1);
}

// ---------------------------------------------------------------------
// 11. проверка на распечатку data elements при неудачном хэше //TODO
// ---------------------------------------------------------------------
void test_not_printing_data_elements_with_hash_error (FILE* tests_file) {
    fprintf (tests_file, "\n\n(11) test_verify_detects_broken_data:\n\n");

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);
    stack_push (&stk, 10);
    stk.hash = 888;

    fprintf (tests_file, "\nCheck absence of data elements in dump\n\n");

    STACK_DUMP_TESTS (tests_file,  &stk);

    free (stk.data - 1);
}

// --------------------------------------------------------------------------
// проверочка
// --------------------------------------------------------------------------
void check (bool condition, const char* message, int line, FILE* tests_file) {
    tests_run++;

    if (condition) {
        tests_passed++;
        fprintf (tests_file, "  [OK]   %s\n\n", message);
    }

    else fprintf (tests_file, "  [FAIL] %s  (line %d)\n\n", message, line);

    fflush (tests_file);
}

void stack_dump_tests_func (FILE* tests_file, const stack_t* stk, const char* func, int line) {
    if (tests_file == NULL) {
        fprintf (stderr, "Cannot open debug file %s\n", __FILE__);
        return;
    }

    struct timeval tv;
    gettimeofday (&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm* local_time = localtime (&now);
    char time_str[64];
    strftime (time_str, sizeof (time_str), "%H:%M:%S", local_time);

    fprintf (tests_file, "[%s.%06ld] =========== STACK DEBUG DUMP ===========\n",
             time_str, tv.tv_usec);
    fprintf (tests_file, "File:      %s\n", __FILE__);
    fprintf (tests_file, "Function:  %s\n", func);
    fprintf (tests_file, "Line:      %d\n", line);
    fprintf (tests_file, "Struct:    stack_t at [%p]\n\n", (const void*)stk);

    if (stk == NULL) {
        fprintf (tests_file, "Stack pointer is NULL\n");
        fclose (tests_file);
        return;
    }

    fprintf (tests_file, "struct stack_t {\n");
    fprintf (tests_file, "    struct address: %p\n", (const void*)stk);
    fprintf (tests_file, "    data address:   %p\n", (const void*)stk->data);

    if (stk->data != NULL && stk->capacity > 0 && (stk->hash == dgb2_hash (stk)))// TODO + test
        print_data_elements (tests_file, stk->data, stk->capacity, stk->size);

    fprintf (tests_file, "\n    size:           %zd\n", stk->size);
    fprintf (tests_file, "    capacity:       %zd\n", stk->capacity);

    #ifndef NDEBUG
        fprintf (tests_file, "    canary left:    0x%llX ", stk->canary_left);
        stackerr_t left_canary_check = check_first_struct_canary (stk->canary_left);
        if (left_canary_check == STACK_OK) fprintf (tests_file, " (ALIVE)\n");
        else fprintf (tests_file, "(NEED YOUR HELP, PROGRAMMER)\n");

        fprintf (tests_file, "    canary right:   0x%llX ", stk->canary_right);
        stackerr_t right_canary_check = check_second_struct_canary (stk->canary_right);
        if (right_canary_check == STACK_OK) fprintf (tests_file,  "(ALIVE)\n");
        else fprintf (tests_file, "(NEED YOUR HELP, PROGRAMMER)\n");

        uint32_t expected_hash = dgb2_hash (stk);
        fprintf (tests_file, "    hash:           0x%08X (stored 0x%08X) ",
                 expected_hash, stk->hash);

        if (stk->hash == expected_hash)
            fprintf (tests_file, "(HASH IS VALID)\n");
        else
            fprintf (tests_file, "(HASH MISMATCH - STRUCTURE DAMAGED)\n");
    #endif

    fprintf (tests_file, "}\n");

    if (stk->size > stk->capacity)
        fprintf (tests_file, "WARNING: size > capacity\n");
    if (stk->size < 0)
        fprintf (tests_file, "Stack has unusual size...\n");
    if (stk->data == NULL)
        fprintf (tests_file, "ERROR: data pointer is NULL\n");
}
