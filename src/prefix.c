#include "prefix.h"
#include "patricia_core.h"
#include "prefix_bits.h"

int add(unsigned int base, char mask){
    int m = (int)mask;
    if (m < 0 || m > 32) return -1;
    if (!base_is_normalized((uint32_t)base, (unsigned)m)) return -1;
    return pat_add((uint32_t)base, (unsigned)m);
}

int del(unsigned int base, char mask){
    int m = (int)mask;
    if (m < 0 || m > 32) return -1;
    if (!base_is_normalized((uint32_t)base, (unsigned)m)) return -1;
    return pat_del((uint32_t)base, (unsigned)m);
}


char check(unsigned int ip){
    int best = pat_check((uint32_t)ip);
    return (char)best;
}

void clear_all(void){
    pat_clear();
}
