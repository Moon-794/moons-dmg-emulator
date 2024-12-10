#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow(gb::mmu* mmu) : memory(mmu)
{
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE), "My window");

    shades[0] = sf::Color(160, 160, 160, 255);
    shades[1] = sf::Color(120, 120, 120, 255);
    shades[2] = sf::Color(80, 80, 80, 255);
    shades[3] = sf::Color(40, 40, 40, 255);

    tex.create(8, 8);
    sprite.setTexture(tex);
    sprite.setScale(16, 16);
}

void gb::RenderWindow::Update()
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

    tex.update(pixels);

    window.clear();
    window.draw(sprite);
    window.display();
}