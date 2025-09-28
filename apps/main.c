#include <stdio.h>
#include <stdint.h>
#include "prefix.h"

static uint32_t ip4(unsigned a,unsigned b,unsigned c,unsigned d){
    return (a<<24)|(b<<16)|(c<<8)|d;
}

static void print_ip(uint32_t ip){
    unsigned a = (ip >> 24) & 0xFF;
    unsigned b = (ip >> 16) & 0xFF;
    unsigned c = (ip >> 8)  & 0xFF;
    unsigned d = ip & 0xFF;
    printf("%u.%u.%u.%u", a,b,c,d);
}

static void add_and_show(uint32_t base, unsigned mask){
    add(base,(char)mask);
    print_ip(base);
    printf("/%u added\n", mask);
}

int main(void){
    clear_all();

    printf(" Adding prefixes\n");
    add_and_show(ip4(10,20,0,0),16);
    add_and_show(ip4(32,64,128,0),20);
    add_and_show(ip4(10,20,1,0),24);
    add_and_show(0u,0);
    add_and_show(ip4(192,168,1,1),32);

    printf("\n Checking addresses\n");
    uint32_t test_ips[] = {
        ip4(10,20,1,5),
        ip4(10,20,200,10),
        ip4(32,64,130,1),
        ip4(1,2,3,4),
        ip4(192,168,1,1),
        ip4(192,168,1,2),
        ip4(0,0,0,0),
        ip4(255,255,255,255)
    };
    for (unsigned i=0;i<sizeof(test_ips)/sizeof(test_ips[0]);i++){
        print_ip(test_ips[i]);
        int mask = (int)(signed char)check(test_ips[i]);
        printf(" Longest prefix /%d\n", mask);
    }

    printf("\n Removing prefix 10.20.0.0/16\n");
    del(ip4(10,20,0,0),16);
    print_ip(ip4(10,20,200,10));
    printf(" After removing /16, check = /%d\n",
           (int)(signed char)check(ip4(10,20,200,10)));

    printf("\n Adding and removing edge cases\n");
    add_and_show(0u,0);
    add_and_show(ip4(255,255,255,255),32);

    print_ip(ip4(255,255,255,255));
    printf(" Check = /%d\n", (int)(signed char)check(ip4(255,255,255,255)));

    del(ip4(255,255,255,255),32);
    print_ip(ip4(255,255,255,255));
    printf(" After removing /32, check = /%d\n",
           (int)(signed char)check(ip4(255,255,255,255)));

    del(0u,0);
    print_ip(ip4(1,2,3,4));
    printf(" After removing /0, check = /%d\n",
           (int)(signed char)check(ip4(1,2,3,4)));

    return 0;
}
