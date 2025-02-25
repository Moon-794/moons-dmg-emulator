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
    struct PPUState 
    {
        uint8_t mode;
        uint32_t clock;
        uint32_t scanline;
        uint32_t cycles;
        const std::vector<Object>& objs;
    };

    struct SpriteData
    {

        SpriteData(gb::mmu& mmu, uint16_t memoryOffset)
        {
            byteOne = mmu.read(memoryOffset);
            byteTwo = mmu.read(memoryOffset + 1);
        }

        uint8_t GetShadeIndex(uint8_t offset)
        {
            byteOne = (byteOne >> (7 - offset)) & 0x01;
            byteTwo = (byteTwo >> (7 - offset)) & 0x01;

            return byteOne + byteTwo;
        };

        uint8_t byteOne;
        uint8_t byteTwo;
    };

    //Contains the SFML structures and handles pixel drawing logic
    class RenderWindow
    {
    public:
        RenderWindow(gb::mmu* mmu);

        void DrawPixels(int count);
        void Update(const PPUState& state);
        void PollWindowEvents();

        int index = 0;

    private:
        sf::RenderWindow window;

        uint32_t xPos = 0; 
        uint32_t yPos = 0;

        uint8_t TexturePixels[160 * 144 * 4];
        sf::Texture tex;
        sf::Sprite sprite;
        sf::Color shades[4];

        gb::mmu* memory;

        uint8_t reverseByte(uint8_t& b);
        void SetPixels(const PPUState& state, sf::Color shade);
    };
}