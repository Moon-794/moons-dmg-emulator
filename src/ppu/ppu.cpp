#include "ppu/ppu.h"

gb::ppu::ppu(gb::mmu* memory)
{
    this->memory = memory;
}

void gb::ppu::Step(uint32_t cycles)
{
    int overflow = 0;
    clock += cycles;

    if(clock >= 456)
    {
        overflow = clock - 456;
        clock = 0;
        scanline++;

        if(scanline >= 144)
        {
            if(scanline == 154)
            {
                //Vertical blank period is over, reset scanline
                scanline = 0;
            }
            
            if(scanline == 144)
            {
                //Entering Vertical blank, change mode
                ChangeMode(ppuMode::VERTICAL_BLANK);
            }
        }  
    }

    UpdateLY();

    //Mode 2, 3 and 0 changes
    if(mode == ppuMode::HORIZONTAL_BLANK && clock == 0)
    {
        //Entered OAM Scan
        ChangeMode(ppuMode::OAM_SCAN);
    }

    if(mode == ppuMode::OAM_SCAN && clock > OAM_SCAN_SPAN)
    {
        //Entered pixel drawing mode
        ChangeMode(ppuMode::DRAWING_PIXELS);
    }

    //if(mode == DRAWING_PIXELS && )

    window.Update();

    if(overflow != 0)
    {
        Step(overflow);
    }
}

void gb::ppu::ChangeMode(ppuMode newMode)
{
    mode = newMode;
}

void gb::ppu::UpdateLY()
{
    memory->write(0xFF44, scanline);
}