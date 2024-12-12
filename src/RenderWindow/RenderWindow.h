#define SFML_STATIC

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define WINDOW_SCALE 4

#include <iostream>

#include "mmu/mmu.h"

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

namespace gb
{
    class ppu;

    class RenderWindow
    {
    public:
        RenderWindow(gb::mmu* mmu, gb::ppu* _ppu);

        void DrawPixels(int count);
        void DrawTile(uint8_t index);
        void Update();

    private:
        sf::RenderWindow window;
        sf::Texture tex;
        sf::Sprite sprite;
        sf::Color shades[4];

        gb::mmu* memory;
        gb::ppu* __ppu;

        //Used to track the current pixel position;
        int x = 0;
        int y = 0;
    };
}