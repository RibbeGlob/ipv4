#pragma once
#include <stdint.h>

#ifndef PREFIX_MAX_NODES
#define PREFIX_MAX_NODES 4096u
#endif

int pat_add  (uint32_t base, unsigned maskLen);
int pat_del  (uint32_t base, unsigned maskLen);
int pat_check(uint32_t ip); 
void pat_clear(void);
