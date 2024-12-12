#define SFML_STATIC

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define WINDOW_SCALE 4

#define OAM 0
#define DRAWPIXELS 1
#define H_BLANK 2
#define V_BLANK 3

#include <iostream>

#include "mmu/mmu.h"

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

namespace gb
{
    class RenderWindow
    {
    public:
        RenderWindow(gb::mmu* mmu);

        void DrawPixels(int count);
        void DrawTile(uint8_t index);
        void Update(uint8_t mode, uint32_t clock, uint32_t scanline, uint32_t cycles);

    private:
        sf::RenderWindow window;

        uint8_t TexturePixels[160 * 144 * 4];
        sf::Texture tex;
        sf::Sprite sprite;
        sf::Color shades[4];

        gb::mmu* memory;

        int yPos = 0;
        int tileRow = 0;
        int pixelRow = 0;

        int xPos = 0;
        int tileColumn = 0;
        int pixelColumn = 0;
    };
}