#include "prefix_bits.h"
#include <assert.h>

uint32_t mask_bits(unsigned maskLen) {
    if (maskLen == 0)  return 0u;
    if (maskLen >= 32) return 0xFFFFFFFFu;
    return 0xFFFFFFFFu << (32 - maskLen);
}

uint32_t normalize_prefix(uint32_t base, unsigned maskLen) {
    return base & mask_bits(maskLen);
}

int base_is_normalized(uint32_t base, unsigned maskLen) {
    return base == normalize_prefix(base, maskLen);
}

int bit_at(uint32_t value, unsigned position) {
    if (position >= 32) {
        return 0;
    }

    unsigned shift_amount = 31u - position;
    uint32_t shifted = value >> shift_amount;
    uint32_t isolated_bit = shifted & 1u;

    return (int)isolated_bit;
}


unsigned common_prefix_len(uint32_t ip_a, uint32_t ip_b) {
    uint32_t x = ip_a ^ ip_b;
    unsigned shifts = 0;
    while (x != 0) {
        x >>= 1;
        shifts++;
    }
    return 32 - shifts;
}
