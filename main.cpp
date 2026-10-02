#include "stack.h"

// TODO должен ли ассерт вылетать при первой же ошибке или пусть ждет пока напишутся все
// TODO дестрой топ copy функции
// TODO w e f d i with errors
// TODO unit test stack with raspechatka
// TODO hash
// TODO validnost ukazateley
// TODO specificator
// TODO dump - расширенный верификатор

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
    push_and_check (&stk1, 30, __PRETTY_FUNCTION__, __LINE__);
    push_and_check (&stk1, 40, __PRETTY_FUNCTION__, __LINE__);
    push_and_check (&stk1, 50, __PRETTY_FUNCTION__, __LINE__);
    push_and_check (&stk1, 60, __PRETTY_FUNCTION__, __LINE__);
    push_and_check (&stk1, 70, __PRETTY_FUNCTION__, __LINE__);

    pop_and_check (&stk1, __PRETTY_FUNCTION__, __LINE__);
    pop_and_check (&stk1, __PRETTY_FUNCTION__, __LINE__);
    pop_and_check (&stk1, __PRETTY_FUNCTION__, __LINE__);
    pop_and_check (&stk1, __PRETTY_FUNCTION__, __LINE__);


    //stk1.canary_left = 0;
    stack_dump (&stk1, __PRETTY_FUNCTION__, __LINE__);
    stack_verify (&stk1, __PRETTY_FUNCTION__, __LINE__);

    free (stk1.data - 1);
    err = stack_destroy (&stk1);
    if (err != STACK_OK) print_custom_error(&stk1, err, __PRETTY_FUNCTION__, __LINE__);

    stack_dump (&stk1, __PRETTY_FUNCTION__, __LINE__);
    printf ("Finish of program\n");
}

stackerr_t stack_init (stack_t* stk) {
    assert (stk != NULL);

    stk->canary_left  = STRUCT_CANARY_VALUE1;
    stk->canary_right = STRUCT_CANARY_VALUE2;

    stack_elem_t* raw = (stack_elem_t*)calloc (stk->capacity + TWO_FOR_CANARIES, sizeof (stack_elem_t));
    if (raw == NULL) return ERROR_WITH_MEMORY_ALLOCATION;

    stk->data = raw + 1;

    stk->data[-1]            = DATA_CANARY_VALUE1;
    stk->data[stk->capacity] = DATA_CANARY_VALUE2;

    poison_memset (stk->data, 0, stk->capacity);

    return STACK_OK;
}

stackerr_t stack_push (stack_t* stk, stack_elem_t value) {
    STACK_ASSERT (stk);

    if (stk->size >= stk->capacity) {
        stackerr_t err = realloc_stack_up (stk);
        if (err != STACK_OK) return err;
    }

    stk->data[stk->size] = value;
    stk->size++;

    STACK_ASSERT_DBG (stk);
    return STACK_OK;
}
//TODO protect  на валидность проверка
void push_and_check (stack_t* stk, stack_elem_t value, const char* func, int line) {
    assert (stk != NULL);

    stackerr_t err = stack_push (stk, value);
    check_of_pushing (stk, err, func, line);
}

void check_of_pushing (const stack_t* stk, int err, const char* func, int line) {
    STACK_ASSERT (stk);

    if (err != STACK_OK)
    {
        print_custom_error (stk, err, func, line);
        stack_dump (stk, func, line);
    }

    STACK_ASSERT_DBG (stk);
}

stackerr_t stack_pop (stack_t* stk) {
    STACK_ASSERT (stk);

    if (stk->size == 0) {
        return STACK_UNDERFLOW;
    }

    stk->size--;
    stk->data[stk->size] = POISON;

    if (stk->size * SHRINK_COEFFICIENT < stk->capacity && stk->capacity > MIN_CAPACITY) {
        stackerr_t err = realloc_stack_down (stk);
        if (err != STACK_OK) {
            return err;
        }
    }

    STACK_ASSERT (stk);
    return STACK_OK;
}

void pop_and_check (stack_t* stk, const char* func, int line) {
    assert (stk != NULL);

    stackerr_t err = stack_pop (stk);
    if (err != STACK_OK) {
        print_custom_error (stk, err, func, line);
        stack_dump (stk, func, line);
    }

    STACK_ASSERT_DBG (stk);
}

stackerr_t realloc_stack_up (stack_t* stk) {
    STACK_ASSERT (stk);

    size_t new_capacity = stk->capacity == 0 ? 5 : stk->capacity * COEFFICIENT + TWO_FOR_CANARIES;

    stack_elem_t* raw = stk->data - 1;
    stack_elem_t* new_raw = (stack_elem_t*)realloc (raw, (new_capacity + TWO_FOR_CANARIES) * sizeof (stack_elem_t));
    if (new_raw == NULL) return ERROR_WITH_MEMORY_REALLOCATION;

    stk->data     = new_raw + 1;
    stk->capacity = new_capacity;

    stk->data[-1]            = DATA_CANARY_VALUE1;
    stk->data[stk->capacity] = DATA_CANARY_VALUE2;

    poison_memset (stk->data, stk->size, stk->capacity);

    STACK_ASSERT_DBG (stk);
    return STACK_OK;
}

stackerr_t realloc_stack_down (stack_t* stk) {
    STACK_ASSERT (stk);

    size_t new_capacity = stk->capacity / COEFFICIENT;
    if (new_capacity < MIN_CAPACITY) {
        new_capacity = MIN_CAPACITY;
    }

    if (new_capacity >= stk->capacity || stk->size > new_capacity) {
        return STACK_OK;
    }

    stack_elem_t* raw = stk->data - 1;
    stack_elem_t* new_raw = (stack_elem_t*)realloc (raw, (new_capacity + TWO_FOR_CANARIES) * sizeof (stack_elem_t));
    if (new_raw == NULL) return ERROR_WITH_MEMORY_REALLOCATION;

    stk->data     = new_raw + 1;
    stk->capacity = new_capacity;

    stk->data[-1]            = DATA_CANARY_VALUE1;
    stk->data[stk->capacity] = DATA_CANARY_VALUE2;

    poison_memset (stk->data, stk->size, stk->capacity);

    STACK_ASSERT_DBG (stk);
    return STACK_OK;
}

bool stack_verify (const stack_t* stk, const char* func, int line) {
    stack_dump (stk, func, line);

    if (stk == NULL) {
        fprintf (stderr, RED "\nERROR" NO_COLOR " Stack pointer is NULL\n");
        print_error_context (stk, func, line);

        return false;
    }


    if (stk->canary_left != STRUCT_CANARY_VALUE1) {
        print_canary_error ("left struct canary",
                            STRUCT_CANARY_VALUE1, stk->canary_left, func, line);
        print_error_context (stk, func, line);

        return false;
    }

    if (stk->canary_right != STRUCT_CANARY_VALUE2) {
        print_canary_error ("right struct canary",
                            STRUCT_CANARY_VALUE2, stk->canary_right, func, line);
        print_error_context (stk, func, line);

        return false;
    }

    if (stk->capacity == 0) {
        print_custom_error (stk, ERROR_CAPACITY_ZERO, func, line);

        return false;
    }

    if (stk->size < 0) {
        print_custom_error (stk, ERROR_SIZE_LESS_THAN_ZERO, func, line);

        return false;
    }

    if (stk->size > stk->capacity) {
        print_custom_error (stk, ERROR_SIZE_EXCEEDS_CAPACITY, func, line);

        return false;
    }

    if (stk->data == NULL) {
        print_custom_error (stk, ERROR_DATA_IS_NULL, func, line);

        return false;
    }

    if (stk->data[-1] != DATA_CANARY_VALUE1) { //TODO ==
        print_canary_error ("left data canary",
                            (unsigned long long)DATA_CANARY_VALUE1,
                            (unsigned long long)stk->data[-1], func, line);
        print_error_context (stk, func, line);

        return false;
    }

    if (stk->data[stk->capacity] != DATA_CANARY_VALUE2) {
        print_canary_error ("right data canary",
                            (unsigned long long)DATA_CANARY_VALUE2,
                            (unsigned long long)stk->data[stk->capacity], func, line);
        print_error_context (stk, func, line);

        return false;
    }

    int number_of_poison = poison_check (stk);
    if (number_of_poison != 0) {
        print_poison_error (number_of_poison, func, line);
        print_error_context (stk, func, line);

        return false;
    }

    return true;
}

stackerr_t stack_destroy (stack_t* stk) {
    if (stk == NULL) return ERROR_STACK_IS_NULL;

    stk->data = NULL;
    stk->size = POISON;
    stk->capacity = POISON;
    stk->canary_left = POISON;
    stk->canary_right = POISON;
    stk = NULL;

    return STACK_OK;
}

void stack_dump (const stack_t* stk, const char* func, int line) {
    const char* filename = "stack_debug.log";

    FILE* log_file = fopen (filename, "a+");
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

    #ifndef NDEBUG
        fprintf (log_file, "    canary left:    0x%llX ", stk->canary_left);
        stackerr_t left_canary_check = check_first_struct_canary (stk->canary_left);
        if (left_canary_check == STACK_OK) fprintf (log_file, " (ALIVE)\n");
        else fprintf (log_file, "(NEED YOUR HELP, PROGRAMMER)\n");

        fprintf (log_file, "    canary right:   0x%llX ", stk->canary_right);
        stackerr_t right_canary_check = check_second_struct_canary (stk->canary_right);
        if (right_canary_check == STACK_OK) fprintf (log_file,  "(ALIVE)\n");
        else fprintf (log_file, "(NEED YOUR HELP, PROGRAMMER)\n");
    #endif

    fprintf (log_file, "}\n");

    if (stk->size > stk->capacity)
        fprintf (log_file, "WARNING: size > capacity\n");
    if (stk->size < 0)
        fprintf (log_file, "Stack has unusual size...\n");
    if (stk->data == NULL)
        fprintf (log_file, "ERROR: data pointer is NULL\n");

    fprintf (log_file, "========================================\n\n");
    fclose (log_file);
}

void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size) {
    assert (log_file != NULL);
    assert (data != NULL);

    #ifndef NDEBUG
        if (size != 0) fprintf (log_file, "\n    data in data:\n");
    #else
        fprintf (log_file, "\n    data in data:\n");
    #endif

    #ifndef NDEBUG
        fprintf (log_file, "        [-1] canary: <%lg> ", data[-1]);
        stackerr_t left_canary_check = check_first_data_canary (data[-1]);
        if (left_canary_check == STACK_OK) fprintf (log_file, "(ALIVE)\n");
        else fprintf (log_file, "(NEED YOUR HELP, PROGRAMMER)\n");
    #endif

    for (size_t i = 0; i < capacity; i++)
    {
        if (i < size)
            fprintf (log_file, "       *[%zu] element: <%lg>\n", i, data[i]);
        else {
            #ifndef NDEBUG
            fprintf (log_file, "        [%zu] element: <%lg> POISON\n", i, data[i]);
            #endif
        }
    }

    #ifndef NDEBUG
        fprintf (log_file, "        [%zu] canary:  <%lg> ", capacity, data[capacity]);
        stackerr_t right_canary_check = check_second_data_canary (data[capacity]);
        if (right_canary_check == STACK_OK) fprintf (log_file, "(ALIVE)\n");
        else fprintf (log_file, "(NEED YOUR HELP, PROGRAMMER)\n");
    #endif
}

void print_custom_error (const stack_t* stk, int error_code, const char* function_name, int line) {
    const char* message = "error";
    size_t number_of_errors = sizeof (errors) / sizeof (errors[0]);

    for (size_t i = 0; i < number_of_errors; i++) {
        if (errors[i].error_code == error_code) {
            message = errors[i].error_message;
            break;
        }
    }

    fprintf (stderr, RED "ERROR %d: %s.\n" NO_COLOR,
             error_code, message);
    print_error_context (stk, function_name, line);
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

void poison_memset (stack_elem_t* data, size_t from, size_t to) {
    assert (data != NULL);

    for (size_t i = from; i < to; i++)
        data[i] = POISON;
}

int poison_check (const stack_t* stk) {
    assert (stk != NULL);

    int number_of_poison_values = 0;

    if (stk->size == stk->capacity) return 0;

    for (size_t i = stk->size; i < stk->capacity; i++) {
        if (stk->data[i] == POISON) number_of_poison_values++;
    }

    if (number_of_poison_values != (stk->capacity - stk->size)) {
        return number_of_poison_values - (stk->capacity - stk->size);
    }

    else return 0;
}

void print_poison_error (int difference, const char* func, int line) {
    if (difference > 0) fprintf (stderr, "Number of poison values is bigger than supposed by %d\n \
                                 Check %s function in %d line\n", difference, func, line);
    if (difference < 0) fprintf (stderr, "Number of poison values is smaller than supposed by %d\n \
                                 Check %s function in %d line\n", difference, func, line);
}

stackerr_t check_first_data_canary (stack_elem_t canary) {
    if (canary != DATA_CANARY_VALUE1) return ERROR_LEFT_DATA_CANARY;
    else return STACK_OK;
}

stackerr_t check_second_data_canary (stack_elem_t canary) {
    if (canary != DATA_CANARY_VALUE2) return ERROR_RIGHT_DATA_CANARY;
    else return STACK_OK;
}

stackerr_t check_first_struct_canary (unsigned long long canary) {
    if (canary != STRUCT_CANARY_VALUE1) return ERROR_LEFT_STRUCT_CANARY;
    else return STACK_OK;
}

stackerr_t check_second_struct_canary (unsigned long long canary) {
    if (canary != STRUCT_CANARY_VALUE2) return ERROR_RIGHT_STRUCT_CANARY;
    else return STACK_OK;
}

void print_canary_error (const char* which, unsigned long long expected,
                         unsigned long long got, const char* func, int line) {
    fprintf (stderr, RED "ERROR: %s damaged.\n" NO_COLOR
             "Expected 0x%llX, got 0x%llX\n"
             "File: %s, function: %s, line %d\n\n",
             which, expected, got, __FILE__, func, line);
}
