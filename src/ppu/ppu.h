#ifndef GB_PPU
#define GB_PPU

#include <cstdint>
#include <iostream>
#include "mmu/mmu.h"

//The PPU (Picture Processing Unit) is responsible for
//drawing the sprites to the screen. It performs other things such
//as controlling VRAM access

//Each scanline is 456 dots
//OAM Scan - 80 dots
//Drawing Pixels - between 172 and 289 dots
//Horizontal blank - between 87 and 204 dots


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
        ppu(gb::mmu* memory);

        void Step(uint32_t cycles);
        ppuMode GetPPUMode();
    
        uint32_t clock = 0;
        uint32_t scanline = 0; // Must also update LCD register 0xFF44, bootrom is currently stalling as this is not yet implemented
    
    private:
        mmu* memory;

        ppuMode mode = ppuMode::OAM_SCAN;
        void ChangeMode(ppuMode newMode);
        void UpdateLY();
    };
}
#endif