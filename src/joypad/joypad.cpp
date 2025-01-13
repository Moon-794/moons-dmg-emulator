#define SFML_STATIC

#include "joypad/joypad.h"
#include "mmu/mmu.h"
#include <bitset>

gb::Joypad::Joypad(gb::mmu* _mmu)
{
    this->_mmu = _mmu;
    
    //Default inputs
    buttonA = sf::Keyboard::Z;
    buttonB = sf::Keyboard::X;
    start = sf::Keyboard::Enter;
    select = sf::Keyboard::Space;

    left = sf::Keyboard::Left;
    right = sf::Keyboard::Right;
    up = sf::Keyboard::Up;
    down = sf::Keyboard::Down;
}

void gb::Joypad::UpdateInputs()
{
    uint8_t reg = _mmu->read(0xFF00);
    uint8_t uniqueButtons = 0x0F;
    uint8_t dpadButtons = 0x0F;

    uint8_t newValue = 0x0F;

    //Start, Select, B, A
    if (sf::Keyboard::isKeyPressed(start)) { uniqueButtons &= ~BIT_3; } else {uniqueButtons |= BIT_3; }
    if (sf::Keyboard::isKeyPressed(select)) { uniqueButtons &= ~BIT_2; } else {uniqueButtons |= BIT_2;}
    if (sf::Keyboard::isKeyPressed(buttonB)) { uniqueButtons &= ~BIT_1; } else {uniqueButtons |= BIT_1;}
    if (sf::Keyboard::isKeyPressed(buttonA)) { uniqueButtons &= ~BIT_0; } else {uniqueButtons |= BIT_0;}

    //Down, Up, Left, Right
    if (sf::Keyboard::isKeyPressed(down)) { dpadButtons &= ~BIT_3; } else {dpadButtons |= BIT_3; }
    if (sf::Keyboard::isKeyPressed(up)) { dpadButtons &= ~BIT_2; } else {dpadButtons |= BIT_2;}
    if (sf::Keyboard::isKeyPressed(left)) { dpadButtons &= ~BIT_1; } else {dpadButtons |= BIT_1;}
    if (sf::Keyboard::isKeyPressed(right)) { dpadButtons &= ~BIT_0; } else {dpadButtons |= BIT_0;}

    if(((reg & BIT_5) != 0) && ((reg & BIT_4) == 0))
        newValue = dpadButtons;

    if(((reg & BIT_5) == 0) && ((reg & BIT_4) != 0))
        newValue = uniqueButtons;
    
    _mmu->mem[0xFF00] = (_mmu->mem[0xFF00] & 0xF0) | (newValue & 0x0F);
}
        
void gb::Joypad::PrintJoypadStatus()
{

}
