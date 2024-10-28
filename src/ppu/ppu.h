#ifndef GB_PPU
#define GB_PPU

#include <cstdint>


namespace gb
{
    //ppu is responsible for interpreting VRAM to display the correct graphics
    //might also house the SDL stuff, not decided yet
    
    class ppu
    {
    public:
        void Step(uint32_t cycles);
    
        uint32_t clock;
    };
}
#endif