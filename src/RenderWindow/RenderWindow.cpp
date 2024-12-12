#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow(gb::mmu* mmu) : memory(mmu)
{
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE), "My window");

    shades[0] = sf::Color(160, 160, 160, 255);
    shades[1] = sf::Color(120, 120, 120, 255);
    shades[2] = sf::Color(80, 80, 80, 255);
    shades[3] = sf::Color(40, 40, 40, 255);

    tex.create(160, 144);
    sprite.setTexture(tex);
    sprite.setScale(4, 4);
}

void gb::RenderWindow::Update(uint8_t mode, uint32_t clock, uint32_t scanline, uint32_t cycles)
{
    if(mode == DRAWPIXELS && scanline == 0)
    {
        int yPos = scanline;
        int tileRow = scanline / 8;
        int pixelRow = scanline % 8;

        int xPos = clock - 80;

        if(xPos > 11)
        {
            
        }

        std::cout << xPos << "\n";
    }

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