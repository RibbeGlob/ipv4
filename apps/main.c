#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "prefix.h"

static uint32_t ip4(unsigned a,unsigned b,unsigned c,unsigned d){
    return (a<<24)|(b<<16)|(c<<8)|d;
}

int main(void) {
    clear_all();

    // Test 1: Add /16 and check
    assert(add(ip4(10,20,0,0), 16) == 0);
    assert((int)(signed char)check(ip4(10,20,123,45)) == 16);

    // Test 2: Add more specific /24
    assert(add(ip4(10,20,1,0), 24) == 0);
    assert((int)(signed char)check(ip4(10,20,1,5)) == 24);

    // Test 3: Remove parent /16, child /24 must stay
    assert(del(ip4(10,20,0,0), 16) == 0);
    assert((int)(signed char)check(ip4(10,20,1,5)) == 24);
    assert((int)(signed char)check(ip4(10,20,200,10)) == -1);

    // Test 4: /0 catch-all
    assert(add(0u, 0) == 0);
    assert((int)(signed char)check(ip4(1,2,3,4)) == 0);

    // Test 5: /32 exact match
    assert(add(ip4(192,168,1,1), 32) == 0);
    assert((int)(signed char)check(ip4(192,168,1,1)) == 32);
    assert((int)(signed char)check(ip4(192,168,1,2)) == 0);

    // Test 6: Remove /32, fallback to /0
    assert(del(ip4(192,168,1,1), 32) == 0);
    assert((int)(signed char)check(ip4(192,168,1,1)) == 0);

    // Test 7: Clear all, nothing should remain
    clear_all();
    assert((int)(signed char)check(ip4(1,2,3,4)) == -1);

    printf("All unit tests passed.\n");
    return 0;
}
