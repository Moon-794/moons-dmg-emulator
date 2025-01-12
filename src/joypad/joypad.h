#ifndef GB_JOYPAD
#define GB_JOYPAD

#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <iostream>

#define BIT_7 0x80
#define BIT_6 0x40
#define BIT_5 0x20
#define BIT_4 0x10
#define BIT_3 0x08
#define BIT_2 0x04
#define BIT_1 0x02
#define BIT_0 0x01

//Joypad is responsible for handling input, and keeping track of keybindings
namespace gb
{
    class mmu;

    class Joypad
    {
    public:
        Joypad(gb::mmu* _mmu);

        void UpdateInputs();
        void PrintJoypadStatus();

        sf::Keyboard::Key up, down, left, right;
        sf::Keyboard::Key buttonA, buttonB, start, select;

    private:
        void RebindKey(sf::Keyboard::Key* keyToChange, sf::Keyboard::Key newKey);

        mmu* _mmu;
    };
}

#endif