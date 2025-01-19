#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow(gb::mmu* mmu) : memory(mmu)
{
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE), "My window");
    window.setFramerateLimit(400);

    shades[0] = sf::Color(160, 160, 160, 255);
    shades[1] = sf::Color(120, 120, 120, 255);
    shades[2] = sf::Color(80, 80, 80, 255);
    shades[3] = sf::Color(40, 40, 40, 255);

    tex.create(160, 144);
    sprite.setTexture(tex);
    sprite.setScale(4, 4);
}

void gb::RenderWindow::Update(uint8_t mode, uint32_t clock, uint32_t scanline, uint32_t cycles, std::vector<Object> objs)
{
    if(mode == DRAWPIXELS && clock < 240)
    {
        yPos = (scanline + memory->read(0xFF42)) % 256;

        tileRow = yPos / 8;
        pixelRow = yPos % 8;

        xPos = clock - 80;
        tileColumn = xPos / 8;
        pixelColumn = xPos % 8;
        
        uint8_t tileIndex = memory->read(0x9800 + tileRow * 32 + tileColumn);

        uint8_t byteOne = memory->read(0x8000 + (tileIndex * 16) + (pixelRow * 2));
        uint8_t byteTwo = memory->read(0x8000 + (tileIndex * 16) + ((pixelRow * 2) + 1));

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
        if(xPos < objs[i].xPos && xPos >= objs[i].xPos - 8)
        {
            uint8_t objTileIndex = objs[i].tileIndex;

            uint8_t byteOne = memory->read((0x8000 + (objTileIndex * 16) + ((scanline + 16) - objs[i].yPos) * 2));
            uint8_t byteTwo = memory->read((0x8000 + (objTileIndex * 16) + ((scanline + 16) - objs[i].yPos) * 2) + 1);

            byteOne = (byteOne >> (7 - (xPos - objs[i].xPos + 8)) & 0x01);
            byteTwo = (byteTwo >> (7 - (xPos - objs[i].xPos + 8)) & 0x01);

            sf::Color shade = shades[byteOne + byteTwo];

            int textureIndex = ((scanline * 160) + xPos) * 4;
            TexturePixels[textureIndex] = shade.r;
            TexturePixels[textureIndex + 1] = shade.g;
            TexturePixels[textureIndex + 2] = shade.b;
            TexturePixels[textureIndex + 3] = shade.a;
        }
    }

    if(scanline == 144 && clock == 455)
    {
        tex.update(TexturePixels);

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

void gb::RenderWindow::PollWindowEvents()
{
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

void gb::RenderWindow::DrawPixels(int count)
{
    //std::cout << "Drawing pixels...\n";
}

void gb::RenderWindow::DrawTile(uint8_t index)
{
    /*

    */
}