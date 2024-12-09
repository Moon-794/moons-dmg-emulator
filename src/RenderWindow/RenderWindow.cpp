#include "RenderWindow/RenderWindow.h"

gb::RenderWindow::RenderWindow()
{
    frameBuffer.create(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE);
    window.create(sf::VideoMode(SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE), "My window");
}

void gb::RenderWindow::Update()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void gb::RenderWindow::DrawPixels(int count)
{
    std::cout << "Drawing pixels...\n";
}