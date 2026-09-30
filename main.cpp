#include "stack.h"

// TODO обработка канареек
// TODO дестрой топ функции
// TODO w e f d i with errors

int main () {
    stack_t stk1 = {};
    stk1.capacity = 5;

    stackerr_t err = stack_init (&stk1);
    if (err != STACK_OK) {
        print_custom_error (err, __FUNCTION__, __LINE__);
        return 1;
    }

    printf ("Start is ok\n");
    printf ("Start capacity: %zu\n", stk1.capacity);
    printf ("Data ptr: %p\n\n", (void*)stk1.data);

    stack_push (&stk1, 10);
    stack_push (&stk1, 20);
    stack_push (&stk1, 30);
    stack_push (&stk1, 40);
    stack_push (&stk1, 50);
    stack_push (&stk1, 60);
    stack_push (&stk1, 70);

    stack_pop (&stk1);

    stk1.canary_left = 0;
    stack_dump (&stk1, __FUNCTION__, __LINE__);
    stack_verify (&stk1, __FUNCTION__, __LINE__);

    free (stk1.data - 1);
    printf ("Finish of program\n");
}

stackerr_t stack_init (stack_t* stk) {
    assert (stk != NULL);

    stk->canary_left  = STRUCT_CANARY_VALUE1;
    stk->canary_right = STRUCT_CANARY_VALUE2;

    stack_elem_t* raw = (stack_elem_t*)calloc (stk->capacity + 2, sizeof (stack_elem_t));
    if (raw == NULL) return ERROR_WITH_MEMORY_ALLOCATION;

    stk->data = raw + 1;

    stk->data[-1]            = STACK_CANARY_VALUE1;
    stk->data[stk->capacity] = STACK_CANARY_VALUE2;

    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POIZON;

    return STACK_OK;
}

stackerr_t stack_push (stack_t* stk, stack_elem_t value) {
    STACK_ASSERT (stk);

    if (stk->size >= stk->capacity) {
        stackerr_t err = realloc_stack (stk);
        if (err != STACK_OK) return err;
    }

    stk->data[stk->size] = value;
    stk->size++;

    STACK_ASSERT_DBG (stk);
    return STACK_OK;
}

stackerr_t stack_pop (stack_t* stk) {
    STACK_ASSERT (stk);

    stk->size--;
    stk->data[stk->size] = POIZON;

    STACK_ASSERT (stk);
    return STACK_OK;
}

stackerr_t realloc_stack (stack_t* stk) {
    STACK_ASSERT (stk);

    size_t new_capacity = stk->capacity == 0 ? 5 : stk->capacity * 2 + 2;

    stack_elem_t* raw = stk->data - 1;
    stack_elem_t* new_raw = (stack_elem_t*)realloc (raw, (new_capacity + 2) * sizeof (stack_elem_t));
    if (new_raw == NULL) return ERROR_WITH_MEMORY_REALLOCATION;

    stk->data     = new_raw + 1;
    stk->capacity = new_capacity;

    stk->data[-1]            = STACK_CANARY_VALUE1;
    stk->data[stk->capacity] = STACK_CANARY_VALUE2;

    for (size_t i = stk->size; i < stk->capacity; i++)
        stk->data[i] = POIZON;

    STACK_ASSERT_DBG (stk);
    return STACK_OK;
}

bool stack_verify (const stack_t* stk, const char* func, int line) {
    if (stk == NULL) {
        fprintf (stderr, RED "\nERROR" NO_COLOR " Stack pointer is NULL\n");
        fprintf (stderr, "File: %s, function: %s, line %d\n\n", __FILE__, func, line);
        return false;
    }

    stack_dump (stk, func, line);

    if (stk->canary_left != STRUCT_CANARY_VALUE1) {
        print_canary_error ("left struct canary",
                            STRUCT_CANARY_VALUE1, stk->canary_left, func, line);
        return false;
    }
    if (stk->canary_right != STRUCT_CANARY_VALUE2) {
        print_canary_error ("right struct canary",
                            STRUCT_CANARY_VALUE2, stk->canary_right, func, line);
        return false;
    }
    if (stk->capacity == 0) {
        print_custom_error (ERROR_CAPACITY_ZERO, func, line);
        return false;
    }
    if (stk->size < 0) {
        print_custom_error (ERROR_SIZE_LESS_THAN_ZERO, func, line);
        return false;
    }
    if (stk->size > stk->capacity) {
        print_custom_error (ERROR_SIZE_EXCEEDS_CAPACITY, func, line);
        return false;
    }
    if (stk->data == NULL) {
        print_custom_error (ERROR_DATA_IS_NULL, func, line);
        return false;
    }

    if (!equal_double (stk->data[-1], STACK_CANARY_VALUE1)) {
        print_canary_error ("left data canary",
                            (unsigned long long)STACK_CANARY_VALUE1,
                            (unsigned long long)stk->data[-1], func, line);
        return false;
    }
    if (!equal_double (stk->data[stk->capacity], STACK_CANARY_VALUE2)) {
        print_canary_error ("right data canary",
                            (unsigned long long)STACK_CANARY_VALUE2,
                            (unsigned long long)stk->data[stk->capacity], func, line);
        return false;
    }

    return true;
}

void stack_dump (const stack_t* stk, const char* func, int line) {
    const char* filename = "stack_debug.log";

    FILE* log_file = fopen (filename, "a");
    if (log_file == NULL) {
        fprintf (stderr, "Cannot open debug file %s\n", filename);
        return;
    }

    struct timeval tv;
    gettimeofday (&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm* local_time = localtime (&now);
    char time_str[64];
    strftime (time_str, sizeof (time_str), "%H:%M:%S", local_time);

    fprintf (log_file, "[%s.%06ld] =========== STACK DEBUG DUMP ===========\n",
             time_str, tv.tv_usec);
    fprintf (log_file, "File:      %s\n", __FILE__);
    fprintf (log_file, "Function:  %s\n", func);
    fprintf (log_file, "Line:      %d\n", line);
    fprintf (log_file, "Struct:    stack_t at [%p]\n\n", (const void*)stk);

    if (stk == NULL) {
        fprintf (log_file, "Stack pointer is NULL\n");
        fclose (log_file);
        return;
    }

    fprintf (log_file, "struct stack_t {\n");
    fprintf (log_file, "    struct address: %p\n", (const void*)stk);
    fprintf (log_file, "    data address:   %p\n", (const void*)stk->data);

    if (stk->data != NULL && stk->capacity > 0)
        print_data_elements (log_file, stk->data, stk->capacity, stk->size);

    fprintf (log_file, "\n    size:           %zu\n", stk->size);
    fprintf (log_file, "    capacity:       %zu\n", stk->capacity);
    fprintf (log_file, "    canary left:    0x%llX\n", stk->canary_left);
    fprintf (log_file, "    canary right:   0x%llX\n", stk->canary_right);
    fprintf (log_file, "}\n");

    if (stk->size > stk->capacity)
        fprintf (log_file, "WARNING: size > capacity\n");
    if (stk->size < 0)
        fprintf (log_file, "Stack has unusual size...\n");
    if (stk->data == NULL)
        fprintf (log_file, "WARNING: data pointer is NULL\n");

    fprintf (log_file, "========================================\n\n");
    fclose (log_file);
}

void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size) {
    assert (log_file != NULL);
    assert (data != NULL);

    fprintf (log_file, "\n    data in data:\n");
    fprintf (log_file, "        [-1] canary: <%lg>\n", data[-1]);

    for (size_t i = 0; i < capacity; i++)
    {
        if (i < size)
            fprintf (log_file, "       *[%zu] element: <%lg>\n", i, data[i]);
        else
            fprintf (log_file, "        [%zu] element: <%lg>\n", i, data[i]);
    }

    fprintf (log_file, "        [%zu] canary: <%lg>\n", capacity, data[capacity]);
}

void print_custom_error (int error_code, const char* function_name, int line) {
    const char* message = "error";
    size_t number_of_errors = sizeof (errors) / sizeof (errors[0]);

    for (size_t i = 0; i < number_of_errors; i++) {
        if (errors[i].error_code == error_code) {
            message = errors[i].error_message;
            break;
        }
    }

    fprintf (stderr, RED "ERROR %d: %s.\n" NO_COLOR
             "File: %s, function: %s, line %d\n\n",
             error_code, message, __FILE__, function_name, line);
}

void print_error_context (const stack_t* stk, const char* func, int line) {
    fprintf (stderr,
             "Error context:\n"
             "  file:      %s\n"
             "  function:  %s\n"
             "  line:      %d\n"
             "  struct:    stack_t at %p\n"
             "  data:      %p\n"
             "  size:      %zu\n"
             "  capacity:  %zu\n"
             "  canary left:  0x%llX\n"
             "  canary right: 0x%llX\n\n",
             __FILE__, func, line,
             (const void*)stk, (const void*)stk->data,
             stk->size, stk->capacity,
             stk->canary_left, stk->canary_right);
}

void print_canary_error (const char* which, unsigned long long expected,
                         unsigned long long got, const char* func, int line) {
    fprintf (stderr, RED "ERROR: %s damaged.\n" NO_COLOR
             "Expected 0x%llX, got 0x%llX\n"
             "File: %s, function: %s, line %d\n\n",
             which, expected, got, __FILE__, func, line);
}

void push_and_check (stack_t* stk, stack_elem_t value, const char* func, int line) {
    assert (stk != NULL);

    stackerr_t err = stack_push (stk, value);
    check_of_pushing (stk, err, func, line);
}

void check_of_pushing (const stack_t* stk, int err, const char* func, int line) {
    STACK_ASSERT (stk);

    if (err != STACK_OK)
    {
        print_custom_error (err, func, line);
        stack_dump (stk, func, line);
    }

    STACK_ASSERT_DBG (stk);
}

bool equal_double (double n1, double n2) {
    if (!isfinite (n1) || !isfinite (n2)) return false;
    return fabs (n1 - n2) < ALMOST_ZERO_VALUE;
}

void poizon_memset (stack_elem_t* data, size_t from, size_t to) {
    assert (data != NULL);

    for (size_t i = from; i < to; i++)
        data[i] = POIZON;
}

