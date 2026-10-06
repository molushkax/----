#ifndef HASH_H
    #define HASH_H

uint32_t   hash_one_byte     (uint32_t h, uint8_t byte);
uint32_t   hash_u64          (uint32_t h, uint64_t value);
uint32_t   dgb2_hash         (const stack_t* stk);
void       stack_update_hash (stack_t* stk);

#endif
