#include "ppu/ppu.h"

gb::ppu::ppu(gb::mmu* memory) : window(memory)
{
    this->memory = memory;
}

void gb::ppu::Step(uint32_t cycles)
{
    uint8_t LCDC = memory->read(0xFF40);

    uint8_t LCDC_CTRL = LCDC >> 7;

    for (size_t i = 0; i < cycles; i++)
    {
        if(LCDC_CTRL == 1)
        {
            clock++;;

            if(clock == 456)
            {
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

                        memory->write(0xFF41, 1);

                        //Enable IF VBlank flag
                        uint8_t IF = memory->read(0xFF0F);
                        IF = IF | 0x01;
                        memory->write(0xFF0F, IF);
                    }
                }  
            }

            UpdateLY();

            if(mode == ppuMode::VERTICAL_BLANK && scanline < 144)
            {
                if(clock < 80)
                    ChangeMode(ppuMode::OAM_SCAN);

                memory->write(0xFF41, 2);
            }

            //Mode 2, 3 and 0 changes
            if(mode == ppuMode::HORIZONTAL_BLANK && clock == 0)
            {
                //Entered OAM Scan
                ChangeMode(ppuMode::OAM_SCAN);

                memory->write(0xFF41, 2);
            }

            if(mode == ppuMode::OAM_SCAN && clock == OAM_SCAN_SPAN)
            {
                //Entered pixel drawing mode
                ChangeMode(ppuMode::DRAWING_PIXELS);

                memory->write(0xFF41, 3);
            }

            if(mode == DRAWING_PIXELS && clock == OAM_SCAN_SPAN + DRAWING_PIXELS_SPAN)
            {
                //Entered Horizontal Blank Mode
                ChangeMode(ppuMode::HORIZONTAL_BLANK);

                memory->write(0xFF41, 0);
            }
        }
        ModeUpdate(cycles);
        window.Update(mode, clock, scanline, cycles, objects);
    }
}

void gb::ppu::ChangeMode(ppuMode newMode)
{
    mode = newMode;

    if(mode == ppuMode::OAM_SCAN)
        OAMSearch();
}

void::gb::ppu::ModeUpdate(int cycles)
{
    if(mode == ppuMode::DRAWING_PIXELS)
    {
        window.DrawPixels(cycles);
    }
}

void gb::ppu::OAMSearch()
{
    objects.clear();

    for (size_t i = 0; i < 40; i++)
    {
        uint8_t yPos = memory->read(0xFE00 + (i * 4));

        if(yPos == 0)
            continue;

        uint8_t rowDiff = (scanline) - (yPos - 16);

        if( rowDiff < 8 && rowDiff >= 0)
        {
            uint8_t xPos = memory->read(0xFE01 + (i * 4));
            uint8_t Index = memory->read(0xFE02 + (i * 4));
            uint8_t flags = memory->read(0xFE03 + (i * 4));
            
            Object obj;
            obj.yPos = yPos;
            obj.xPos = xPos;
            obj.tileIndex = Index;
            obj.Flags = flags;

            objects.push_back(obj);

            if(objects.size() == 10)
                return;
        }
    }
}

gb::ppuMode gb::ppu::GetPPUMode()
{
    return mode;
}

void gb::ppu::UpdateLY()
{
    memory->write(0xFF44, scanline);
}