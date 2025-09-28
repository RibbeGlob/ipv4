#include "prefix.h"
#include "patricia_core.h"
#include "prefix_bits.h"

int add(unsigned int base, char mask){
    if (mask < 0 || mask > 32) return -1;

    unsigned m = (unsigned)mask;

    if (!base_is_normalized((uint32_t)base, m)) return -1;

    return pat_add((uint32_t)base, m);
}

int del(unsigned int base, char mask){
    if (mask < 0 || mask > 32) return -1;
    unsigned m = (unsigned)mask;
    if (!base_is_normalized((uint32_t)base, m)) return -1;
    return pat_del((uint32_t)base, m);
}

char check(unsigned int ip){
    int best = pat_check((uint32_t)ip);
    return (char)best;
}

void clear_all(void){
    pat_clear();
}
