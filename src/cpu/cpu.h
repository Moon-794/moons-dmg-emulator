#ifndef GB_CPU
#define GB_CPU

#include <cstdint>

class cpu
{
    //8 Bit cpu registers
    uint8_t a, b, c, d, e, f, h, l;

    //Register F is the "flag" register, and is updated based on certain operation outcomes
    //Bit layout
    //          [unused] 
    //0 0 0 0   0 0 0 0
    //| | | \__ Carry
    //| | \__ Half carry
    //| \__ Subtraction
    //\__ Zero  
};

#endif