#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow(gb::mmu* mmu) : memory(mmu)
{
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE + (8 * 16), SCREEN_HEIGHT * WINDOW_SCALE), "My window");
    window.setFramerateLimit(60);

    shades[0] = sf::Color(160, 120, 120, 255);
    shades[1] = sf::Color(120, 80, 80, 255);
    shades[2] = sf::Color(80, 40, 40, 255);
    shades[3] = sf::Color(40, 00, 00, 255);

    tex.create(160, 144);
    sprite.setTexture(tex);
    sprite.setScale(4, 4);

    debugTex.create(8, 8);
    debugSprite.setTexture(debugTex);
    debugSprite.setScale(16, 16);
    debugSprite.setPosition(SCREEN_WIDTH * WINDOW_SCALE, 0);
}

//Main function to draw pixels to the screen texture, the function is called per pixel to maintain accuracy when scroll registers
//are manipulated mid-scanline
void gb::RenderWindow::Update(uint8_t mode, uint32_t clock, uint32_t scanline, uint32_t cycles, const std::vector<Object>& objs)
{
    if(mode == DRAWPIXELS && clock < 240)
    {
        yPos = (scanline + memory->read(0xFF42)) % 256;

        tileRow = yPos / 8;
        pixelRow = yPos % 8;

        uint8_t scx = memory->read(0xFF43);

        xPos = (clock - 80);

        uint8_t bgX = (xPos + scx) % 256;

        tileColumn = bgX / 8;
        pixelColumn = bgX % 8;
        
        //Changing the addressing mode based on bit 3 and 4 of LCDC register
        uint16_t memArea = (memory->read(0xFF40) & (BIT_3)) != 0 ? 0x9C00 : 0x9800;
        uint8_t tileIndex = memory->read(memArea + tileRow * 32 + tileColumn);
        uint16_t offset;
        if((memory->read(0xFF40) & (BIT_4)) == 0 )
        {
            offset = 0x9000 + (static_cast<int8_t>(tileIndex) * 16) + (pixelRow * 2);
        }
        else
        {
            offset = 0x8000 + (tileIndex * 16) + (pixelRow * 2);
        }

       
        uint8_t byteOne = memory->read(offset);
        uint8_t byteTwo = memory->read(offset + 1);

        byteOne = (byteOne >> (7 - pixelColumn)) & 0x01;
        byteTwo = (byteTwo >> (7 - pixelColumn)) & 0x01;

        sf::Color shade = shades[byteOne + byteTwo];

        int textureIndex = ((scanline * 160) + xPos) * 4;
        TexturePixels[textureIndex] = shade.r;
        TexturePixels[textureIndex + 1] = shade.g;
        TexturePixels[textureIndex + 2] = shade.b;
        TexturePixels[textureIndex + 3] = shade.a;
    }

    for (size_t i = 0; i < objs.size(); i++)
    {
        Object o = objs[i];
        if(xPos < o.xPos && xPos >= o.xPos - 8)
        {
            uint8_t objTileIndex = o.tileIndex;
            uint8_t tileLine = ((scanline + 16) - o.yPos);
            
            //Vertical flip
            if ((o.Flags & BIT_6) == BIT_6)
            {
                tileLine = 7 - tileLine;
            }

            //byte one and byte two contain the tiledata for this row of the sprite
            uint8_t byteOne = memory->read((0x8000 + (objTileIndex * 16) + tileLine * 2));
            uint8_t byteTwo = memory->read((0x8000 + (objTileIndex * 16) + tileLine * 2) + 1);

            //horizontal flip
            if((o.Flags & BIT_5) == BIT_5)
            {
                byteOne = reverseByte(byteOne);
                byteTwo = reverseByte(byteTwo);
            }

            //Extract the current bit we are drawing, and shift to the first position
            byteOne = (byteOne >> (7 - (xPos - o.xPos + 8)) & 0x01);
            byteTwo = (byteTwo >> (7 - (xPos - o.xPos + 8)) & 0x01);

            //The two bits form an index to the shade depth
            sf::Color shade = shades[byteOne + byteTwo];

            int textureIndex = ((scanline * 160) + xPos) * 4;

            if(shade.r == shades[0].r)
            {
                //transparent, dont replace
            }
            else
            {
                TexturePixels[textureIndex] = shade.r;
                TexturePixels[textureIndex + 1] = shade.g;
                TexturePixels[textureIndex + 2] = shade.b;
                TexturePixels[textureIndex + 3] = shade.a;
            }

        }
    }
    
}

uint8_t gb::RenderWindow::reverseByte(uint8_t b) {
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

void gb::RenderWindow::PollWindowEvents()
{
    window.clear();
    DrawTile(index / 8);

    index++;

    if(index > 1024)
    {
        index = 0;
    }

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

void gb::RenderWindow::DrawTile(uint8_t index)
{
    uint8_t pixels[8 * 8 * 4];
    uint8_t tileData[16];

    for (size_t i = 0; i < 16; i++)
    {
        tileData[i] = memory->read(0x8000 + (index * 16) + i);
    }

    for (size_t i = 0; i < 8; i++)
    {
        uint8_t first = tileData[(2 * i)];
        uint8_t second = tileData[(2 * i) + 1];
        for (size_t f = 0; f < 8; f++)
        {
            uint8_t shiftedFirst = (first >> 7 - f) & 0x01;
            uint8_t shiftedSecond = (second >> 7 - f) & 0x01;
            sf::Color shade = shades[shiftedFirst + shiftedSecond];
            size_t pixelIndex = (i * 8 + f) * 4;
            pixels[pixelIndex] = shade.r;
            pixels[pixelIndex + 1] = shade.g;
            pixels[pixelIndex + 2] = shade.b;
            pixels[pixelIndex + 3] = shade.a;
        }
    }

    debugTex.update(pixels);
    window.draw(debugSprite);
}