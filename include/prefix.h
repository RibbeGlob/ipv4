#pragma once
#include <stdint.h>

int add(unsigned int base, char mask);
int del(unsigned int base, char mask);
char check(unsigned int ip);

void clear_all(void);
