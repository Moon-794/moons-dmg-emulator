#define SFML_STATIC

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define WINDOW_SCALE 4

#include <iostream>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

namespace gb
{
    class RenderWindow
    {
    public:
        RenderWindow();

        void DrawPixels(int count);
        void Update();

    private:
        sf::RenderWindow window;

        sf::Image frameBuffer;

        //Used to track the current pixel position;
        int x = 0;
        int y = 0;
    };
}