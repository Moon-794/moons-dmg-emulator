#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow(gb::mmu* mmu) : memory(mmu)
{
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE), "DMG-Emulator");
    //window.setFramerateLimit(600);
    
    shades[0] = sf::Color(200,160,160, 255);
    shades[1] = sf::Color(160,120,120, 255);
    shades[2] = sf::Color(120,80,80, 255);
    shades[3] = sf::Color(80,40,40, 255);

    tex.create(160, 144);
    sprite.setTexture(tex);
    sprite.setScale(4, 4);
}

//Main function to draw pixels to the screen texture, the function is called per pixel to maintain accuracy when scroll registers
//are manipulated mid-scanline
void gb::RenderWindow::Update(const PPUState& state)
{
    if(state.mode == DRAWPIXELS && state.clock < 240)
    {
        yPos = (state.scanline + memory->read(0xFF42)) % 256;
        xPos = (state.clock - 80);

        int tileRow = yPos / 8;
        int pixelRow = (yPos) % 8;

        uint8_t scx = memory->read(0xFF43);
        uint8_t bgX = (xPos + scx) % 256;
        int tileColumn = bgX / 8;
        int pixelColumn = bgX % 8;
        
        //Changing the addressing mode based on bit 3 and 4 of LCDC register
        uint16_t memArea = (memory->read(0xFF40) & (BIT_3)) != 0 ? 0x9C00 : 0x9800;
        uint8_t tileIndex = memory->read(memArea + tileRow * 32 + tileColumn);
        uint16_t offset;
        if((memory->read(0xFF40) & (BIT_4)) == 0 )
        {
            offset = 0x9000 + (static_cast<int8_t>(tileIndex) * 16) + ((pixelRow) * 2);
        }
        else
        {
            offset = 0x8000 + (tileIndex * 16) + ((pixelRow) * 2);
        }

        SpriteData spriteData(*memory, offset);
        sf::Color shade = shades[spriteData.GetShadeIndex(pixelColumn)];
        SetPixels(state, shade);

        for (size_t i = 0; i < state.objs.size(); i++)
        {
            Object o = state.objs[i];
            if(xPos < o.xPos && xPos >= o.xPos - 8)
            {
                uint8_t objTileIndex = o.tileIndex;
                uint8_t tileLine = ((state.scanline + 16) - o.yPos);
                
                //Vertical flip
                if ((o.Flags & BIT_6) == BIT_6)
                {
                    tileLine = 7 - tileLine;
                }

                //byte one and byte two contain the tiledata for this row of the sprite
                uint16_t memoryIndex = (0x8000 + (objTileIndex * 16) + tileLine * 2);
                SpriteData objectData(*memory, memoryIndex);

                //horizontal flip
                if((o.Flags & BIT_5) == BIT_5)
                {
                    reverseByte(objectData.byteOne);
                    reverseByte(objectData.byteTwo);
                }
                
                uint8_t shadeIndex = objectData.GetShadeIndex((xPos - o.xPos + 8));
                sf::Color shade = shades[shadeIndex];

                //Only draw if not transparent
                if(shade.r != shades[0].r)
                {
                    SetPixels(state, shade);
                }
            }
        }
    }
}

//Reverses a byte. Used to modify sprite data for horizontal flipping.
uint8_t gb::RenderWindow::reverseByte(uint8_t& b) 
{
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

//Sets the rgb values for a specific pixel
void gb::RenderWindow::SetPixels(const PPUState& state, sf::Color shade)
{
    uint32_t textureIndex = ((state.scanline * 160) + xPos) * 4;

    TexturePixels[textureIndex]     = shade.r;
    TexturePixels[textureIndex + 1] = shade.g;
    TexturePixels[textureIndex + 2] = shade.b;
    TexturePixels[textureIndex + 3] = shade.a;
}

void gb::RenderWindow::PollWindowEvents()
{
    window.clear();
    tex.update(TexturePixels);
    window.draw(sprite);
    window.display();

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            exit(0);
        }
    }
}