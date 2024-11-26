#ifndef GB_PPU
#define GB_PPU

#include <cstdint>

//The PPU (Picture Processing Unit) is responsible for
//drawing the sprites to the screen. It performs other things such
//as controlling VRAM access

namespace gb
{
    enum ppuMode
    {
        OAM_SCAN,
        DRAWING_PIXELS,
        HORIZONTAL_BLANK,
        VERTICAL_BLANK
    };

    class ppu
    {
    public:
        void Step(uint32_t cycles);
    
        uint32_t clock = 0;
        uint32_t scanline = 0;
    
    private:
        ppuMode mode = ppuMode::OAM_SCAN;
        void ChangeMode(ppuMode newMode);
    };
}
#endif