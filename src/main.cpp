#include <iostream>

#include "ppu/ppu.h"
#include "mem/memory.h"

int main(int arg, char** args)
{
    //Ok, so goal of emulator is to "emulate" the DMG cpu
    //Perform the fetch decode execute cycle
    //Interpret VRAM to draw the game to the screen, using SDL2
    //Capture Input from the player to control the game

    //The cpu, ppu and ram are what sticks out to me the most right now

    memory gb_memory;

    gb_memory.write(0x5700, 12);
    std::cout << (unsigned int)gb_memory.read(0x5700);

    return 0;
}