#include <iostream>

#include "gameboy.h"

int main(int arg, char** args)
{
    //Ok, so goal of emulator is to "emulate" the DMG cpu
    //Perform the fetch decode execute cycle
    //Interpret VRAM to draw the game to the screen, using SDL2
    //Capture Input from the player to control the game

    //The cpu, ppu and ram are what sticks out to me the most right now

    //load rom

    //while running
        //fetch instruction

        //decode

        //execute instruction

        //update screen?

    //Gameboy should take the rom as an arg
    Gameboy gameboy = Gameboy();
    gameboy.Run();

    return 0;
}