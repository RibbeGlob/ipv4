#ifndef PREFIX_BITS_H
#define PREFIX_BITS_H

#include <stdint.h>

uint32_t mask_bits(unsigned maskLen);
uint32_t normalize_prefix(uint32_t base, unsigned maskLen);
int base_is_normalized(uint32_t base, unsigned maskLen);
int bit_at(uint32_t v, unsigned pos);
unsigned common_prefix_len(uint32_t a, uint32_t b);
#endif
