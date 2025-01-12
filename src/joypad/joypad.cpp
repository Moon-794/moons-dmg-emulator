#define SFML_STATIC

#include "joypad/joypad.h"
#include "mmu/mmu.h"

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
    uint8_t newValue = 0x0F;

    if((reg & BIT_5) == 0)
    {
        //Start, Select, B, A
        if (sf::Keyboard::isKeyPressed(start)) { newValue &= ~BIT_3; } else {newValue |= BIT_3; }
        if (sf::Keyboard::isKeyPressed(select)) { newValue &= ~BIT_2; } else {newValue |= BIT_2;}
        if (sf::Keyboard::isKeyPressed(buttonB)) { newValue &= ~BIT_1; } else {newValue |= BIT_1;}
        if (sf::Keyboard::isKeyPressed(buttonA)) { newValue &= ~BIT_0; } else {newValue |= BIT_0;}
    }
    
    if((reg & BIT_4) == 0)
    {
        //Down, Up, Left, Right
        if (sf::Keyboard::isKeyPressed(down)) { newValue &= ~BIT_3; } else {newValue |= BIT_3; }
        if (sf::Keyboard::isKeyPressed(up)) { newValue &= ~BIT_2; } else {newValue |= BIT_2;}
        if (sf::Keyboard::isKeyPressed(left)) { newValue &= ~BIT_1; } else {newValue |= BIT_1;}
        if (sf::Keyboard::isKeyPressed(right)) { newValue &= ~BIT_0; } else {newValue |= BIT_0;}
    }

    _mmu->mem[0xFF00] = (_mmu->mem[0xFF00] & 0xF0) | (newValue & 0x0F);
}
        
void gb::Joypad::PrintJoypadStatus()
{

}
