#include "stack.h"

//-------------------------------------------------------------------------------------------------
//TODO структура ошибок и туда файл линия функция и в разных вариантах ошибок это оформить чтобы туда записывалось
int main () {
    stack_t stk1 = {};
    stk1.capacity = 5;

    int err = stack_init (&stk1);
    if (err != STACK_OK) printf ("Stupid programmist\n");
    else
    {
        printf ("Start is ok\n");
        printf ("Start capacity: %zu\n", stk1.capacity);
        printf ("Data ptr: %p\n\n",  stk1.data);
    }

    stack_elem_t value1 = 10;
    push_and_check (&stk1, value1, __PRETTY_FUNCTION__, __LINE__); //TODO pretty function
    stack_push (&stk1, 20);
    stack_push (&stk1, 30);
    stack_push (&stk1, 40);
    stack_push (&stk1, 50);
    stack_push (&stk1, 60);
    stack_push (&stk1, 70);
    stk1.size = -6;

    stack_pop (&stk1);

    printf ("We don't want to see %d\n", 70);
    printf ("We see               %lg\n", stk1.data[stk1.size]);

    stack_dump (&stk1);
    stack_verify (&stk1, __FUNCTION__, __LINE__);

    free (stk1.data);
}

//-------------------------------------------------------------------------------------------------

stackerr_t stack_init (stack_t* stk) {
    assert (stk != NULL); //TODO assert (stack_verify == 0);

    stk->data = (stack_elem_t*)calloc (stk->capacity, sizeof (stack_elem_t));
    poizon_memset (stk->data, 0, stk->capacity);

    printf ("stk->data[0]: %lg\n", stk->data[0]);
    if (stk->data == NULL) return ERROR_WITH_MEMORY_ALLOCATION;
    else return STACK_OK;
}

stackerr_t stack_push (stack_t* stk, stack_elem_t value) {
    STACK_ASSERT(stk);

    if (stk->size >= stk->capacity)
    {
        realloc_stack (stk);
        //TODO check err
    }

    poizon_memset (stk->data, stk->size, stk->capacity);

    stk->data[stk->size] = value;
    stk->size++;

    printf ("Push: what was written: %lg\n", stk->data[stk->size - 1]);
    printf ("Push: size after pushing: %zu\n", stk->size);
    printf ("Push: capacity after pushing: %zu\n", stk->capacity);
    printf ("\n");

    STACK_ASSERT_DBG(stk);
    //stk->data[stk->size - 1] = 0;
    if (equal_double (stk->data[stk->size - 1], 0)) return ERROR; //TODO name error
    else return STACK_OK;
}

void push_and_check (stack_t* stk, stack_elem_t value, const char* func, int line) {
    assert(stk != NULL);

    int err = stack_push (stk, value);
    check_of_pushing (stk, err, func, line);

    STACK_ASSERT_DBG(stk);
}

stackerr_t stack_pop (stack_t* stk) {
    STACK_ASSERT(stk); //TODO

    stk->data[stk->size - 1] = POIZON;
    printf ("Pop: what was written: %lg\n", stk->data[stk->size - 1]); //TODO pop minus realloc
    stk->size--;

    STACK_ASSERT(stk); //TODO on debug
    return STACK_OK;
}

bool equal_double (double n1, double n2) {
    assert (isfinite (n1));
    assert (isfinite (n2));

    return fabs (n1 - n2) < ALMOST_ZERO_VALUE;
}

stackerr_t realloc_stack (stack_t* stk) {
    STACK_ASSERT(stk);

    size_t new_capacity = stk->capacity == 0 ? 5 : stk->capacity * 2;
    stack_elem_t* new_data = (stack_elem_t*)realloc (stk->data, new_capacity * sizeof(stack_elem_t)); //TODO realloc func
    //TODO почему работает если насильно NULL
    if (new_data == NULL) return ERROR_WITH_MEMORY_REALLOCATION;

    stk->data = new_data;
    stk->capacity = new_capacity;

    STACK_ASSERT_DBG(stk);
    return STACK_OK;
}

bool stack_verify (const stack_t* stk, const char* func, int line) {
    stack_dump (stk);

    if (stk == NULL) {
        stack_dump (stk);
        fprintf(stderr, RED "\nERROR" NO_COLOR " Stack pointer is NULL\n");
        fprintf (stderr, "Mashka warns about an error at %s file, function: %s, %d line\n\n", __FILE__, func, line);
        return false;
    }

    else if (stk->size > stk->capacity) {
        stack_dump (stk);
        fprintf (stderr, RED "\nERROR" NO_COLOR " Stack size (%zu) exceeds capacity (%zu)\n", stk->size, stk->capacity);
        fprintf (stderr, "Mashka warns about an error at %s file, function: %s, %d line\n\n", __FILE__, func, line);
        return false;
    }

    else if (stk->data == NULL && stk->capacity > 0) {
        stack_dump (stk);
        fprintf (stderr, RED "\nERROR" NO_COLOR " Stack data is NULL, but capacity is %zu\n", stk->capacity);
        fprintf (stderr, "Mashka warns about an error at %s file, function: %s, %d line\n\n", __FILE__, func, line);
        return false;
    }

    else return true;
}

void check_of_pushing (const stack_t* stk, int err, const char* func, int line) {
    STACK_ASSERT(stk);

    if (err != STACK_OK)
    {
        print_custom_error (err, func, line);
        stack_dump (stk);
    }

    STACK_ASSERT_DBG(stk);
}

void poizon_memset (stack_elem_t* data, size_t from, size_t to) {
    assert (data != NULL);

    for (size_t i = from; i < to; i++)
    {
        data[i] = POIZON;
    }
}

void stack_dump (const stack_t* stk) {
    assert (stk != NULL);

    const char* filename = "stack_debug.log";

    FILE* log_file = fopen (filename, "a");
    if (log_file == NULL)
    {
        fprintf(stderr, "CRITICAL: Cannot open debug file %s!\n", filename);
        return;
    }

    struct timeval tv;
    gettimeofday (&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm *local_time = localtime (&now);
    char time_str[64];
    strftime (time_str, sizeof(time_str), "%H:%M:%S", local_time);

    if (stk == NULL)
    {
        fprintf(log_file, "[%s.%06ld] " "\nSTACK ERROR: NULL pointer passed!\n", time_str, tv.tv_usec);
        fclose(log_file);
        return;
    }

    fprintf (log_file, "[%s.%06ld]\n=========== STACK DEBUG DUMP ===========\n", time_str, tv.tv_usec);
    fprintf (log_file, "\nstruct stack_t {\n");
    fprintf (log_file, "    struct address: %p\n", (void*)stk);
    fprintf (log_file, "    data address:   %p\n", (void*)stk->data);

    if (stk->size > 0)
    {
        print_data_elements (log_file, stk->data, stk->capacity, stk->size);
    }

    fprintf (log_file, "\n    size:           %zu\n", stk->size);
    fprintf (log_file, "    capacity:       %zu\n", stk->capacity);
    fprintf (log_file, "}\n\n");

    if (stk->size > stk->capacity)
    {
        fprintf(log_file, "!!! WARNING: SIZE > CAPACITY !!!\n\n");
    }

    if (stk->size == 0)
    {
        fprintf(log_file, "Stack is empty :(\n\n");
    }

    if (stk->data == NULL)
    {
        fprintf(log_file, "WARNING: data pointer is NULL (memory was not allocated)\n\n");
    }

    fprintf(log_file, "========================================\n\n");

    fclose(log_file);
}

void print_data_elements (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size) {
    assert (log_file != NULL);
    assert (data != NULL);

    fprintf (log_file, "    data in data:");
    for (size_t i = 0; i < capacity; i++)
    {
        if (i < size) fprintf (log_file, "\n       *[%zu] element: <%lg>", i, data[i]);
        else fprintf (log_file, "\n        [%zu] element: <%lg>", i, data[i]);
    }
}

void print_custom_error (int error_code, const char* function_name, int line) {

    const char* message = "Error";
    size_t number_of_errors = sizeof(errors) / sizeof(errors[0]);

    for (size_t i = 0; i < number_of_errors; i++)
    {
        if (errors[i].error_code == error_code)
        {
            message = errors[i].error_message;
            break;
        }
    }

    fprintf (stderr, RED "ERROR %d %s in function <<%s>>.\nCheck line %d\n\n" NO_COLOR,
             error_code, message, function_name, line);
}
