#include "stack.h"
#include "stack_funcs.h"
#include "tests.h"
#include "hash.h"

// dgb2 хэш функция
// https://habr.com/ru/companies/otus/articles/541378/?ysclid=muvuuczsvq760208665

// хэшируется всё, что определяет состояние стека:
// - обе канарейки структуры,
// - size и capacity,
// - канарейки вокруг data и сами элементы data.
// поле stk->hash НЕ хэшируется — иначе функция зависела бы сама от себя.
uint32_t dgb2_hash (const stack_t* stk) {
    assert (stk != NULL);

    uint32_t h = 5381u;

    h = hash_u64 (h, (uint64_t)stk->canary_left);
    h = hash_u64 (h, (uint64_t)stk->canary_right);

    h = hash_u64 (h, (uint64_t)stk->size);
    h = hash_u64 (h, (uint64_t)stk->capacity);

    if (stk->data != NULL && stk->capacity > 0) {
        uint64_t left_canary = 0;
        memcpy (&left_canary, &stk->data[-1], sizeof (stack_elem_t));
        h = hash_u64 (h, left_canary);

        for (ssize_t i = 0; i < stk->capacity; i++) {
            uint64_t value_bits = 0;
            memcpy (&value_bits, &stk->data[i], sizeof (stack_elem_t));
            h = hash_u64 (h, value_bits);
        }

        uint64_t right_canary = 0;
        memcpy (&right_canary, &stk->data[stk->capacity], sizeof (stack_elem_t));
        h = hash_u64 (h, right_canary);
    }

    return h;
}

// замешивает один байт в текущее значение хэша: h = h * 33 + byte.
uint32_t hash_one_byte (uint32_t h, uint8_t byte) {
    return h * 33 + byte;
}

// замешивает 64-битное число в хэш, идя по его байтам.
uint32_t hash_u64 (uint32_t h, uint64_t value) {
    for (size_t i = 0; i < sizeof (value); i++) {
        h = hash_one_byte (h, (uint8_t)(value >> (8 * i))); //побитовый сдвиг вправо
    }

    return h;
}

// Пересчитывает хэш и записывает его в stk->hash.
void stack_update_hash (stack_t* stk) {
    if (stk == NULL) return;

    stk->hash = dgb2_hash (stk);
}
