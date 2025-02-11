#include "ppu/ppu.h"

gb::ppu::ppu(gb::mmu* memory) : window(memory)
{
    this->memory = memory;
}

void gb::ppu::Step(uint32_t cycles)
{
    uint8_t LCDC = memory->read(0xFF40);
    if(LCDC & BIT_3 == 0)
    {
        std::cout << "chec";
    }
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
            }

            //Mode 2, 3 and 0 changes
            if(mode == ppuMode::HORIZONTAL_BLANK && clock == 0)
            {
                //Entered OAM Scan
                ChangeMode(ppuMode::OAM_SCAN);
            }

            if(mode == ppuMode::OAM_SCAN && clock == OAM_SCAN_SPAN)
            {
                //Entered pixel drawing mode
                ChangeMode(ppuMode::DRAWING_PIXELS);
            }

            if(mode == DRAWING_PIXELS && clock == OAM_SCAN_SPAN + DRAWING_PIXELS_SPAN)
            {
                //Entered Horizontal Blank Mode
                ChangeMode(ppuMode::HORIZONTAL_BLANK);
            }
        }

        window.Update(mode, clock, scanline, cycles, objects);
    }
}

void gb::ppu::ChangeMode(ppuMode newMode)
{
    mode = newMode;
    uint8_t STAT = memory->read(0xFF41);

    if(mode == ppuMode::HORIZONTAL_BLANK)
    {
        //clear bits 0 and 1
        STAT &= ~(1 << 0);
        STAT &= ~(1 << 1);
        
        memory->write(0xFF41, STAT);
        return;
    }

    if(mode == ppuMode::VERTICAL_BLANK)
    {
        //set bit 0 and clear bit 1
        STAT |= (1 << 0);
        STAT &= ~(1 << 1);

        memory->write(0xFF41, STAT);
        return;
    }

    if(mode == ppuMode::OAM_SCAN)
    {
        //clear bit 0 and set bit 1
        STAT &= ~(1 << 0);
        STAT |= (1 << 1);

        memory->write(0xFF41, STAT);
        OAMSearch();
        return;
    }

    if(mode == ppuMode::DRAWING_PIXELS)
    {
        //set bit 0 and set bit 1
        STAT |= (1 << 0);
        STAT |= (1 << 1);

        memory->write(0xFF41, STAT);
        return;
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

void gb::ppu::UpdateLY()
{
    memory->write(0xFF44, scanline);
}