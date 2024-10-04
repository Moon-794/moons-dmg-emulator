#ifndef GB_CPU
#define GB_CPU

#include <cstdint>
#include "mmu/mmu.h"

namespace gb
{  
    class cpu
    {
    public:

        cpu(gb::mmu* memory);
        void Step();

    private:
        //8 Bit cpu registers, f is special and represent flag states from certain ops
        uint8_t a, b, c, d, e, f, h, l;
        
        uint16_t program_counter = 0x0000;
        uint16_t stack_pointer = 0x0000;

        gb::mmu* memory;
    };
}
#endif