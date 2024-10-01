#ifndef GB_MEMORY
#define GB_MEMORY

#define VRAM_START 0x8000
#define VRAM_END 0x9FFF

#include <vector>
#include <cstdint>
#include <stdexcept>

namespace gb
{  
    class mmu
    {
    public:
        mmu();

        //returns a byte from the specified 16 bit address
        uint8_t read(uint16_t address);

        //writes a byte to the specified 16 bit address
        void write(uint16_t address, uint8_t data);

    private:
        std::vector<uint8_t> mem;
    };
}


#endif