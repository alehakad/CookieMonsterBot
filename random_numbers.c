#include "random_numbers.h" /* rand numbers header */


unsigned int random_state = 1804289383;

/* generate 32-bit pseudo legal numbers with xor-shift algorithm */
unsigned int GetRandomU32Number()
{
    unsigned int number = random_state;
    
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    random_state = number;
    
    return number;
}

unsigned long GetRandomU64Number()
{
    unsigned long n1, n2, n3, n4;
    
    n1 = (unsigned long)(GetRandomU32Number()) & 0xFFFF;
    n2 = (unsigned long)(GetRandomU32Number()) & 0xFFFF;
    n3 = (unsigned long)(GetRandomU32Number()) & 0xFFFF;
    n4 = (unsigned long)(GetRandomU32Number()) & 0xFFFF;
    
   
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}