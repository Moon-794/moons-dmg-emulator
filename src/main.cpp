#include <iostream>
#include <fstream>
#include <iomanip>

#include "gameboy.h"

int main(int argc, char** args)
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

    if(argc < 2)
    {
        std::cout << "Error: Pass a ROM file as the first argument" << std::endl;
        return 1;
    }

    char* romFile = args[1];

    std::ifstream fs(romFile, std::ios::in | std::ios::binary);

    if(!fs)
    {
        std::cout << "Failed to open file:" << romFile << std::endl;
        return 1;
    }
    else
    {
        fs.seekg (0, fs.end);
        int length = fs.tellg();
        fs.seekg (0, fs.beg);

        unsigned char* buffer = new unsigned char[length];
        fs.read((char*)buffer, length);

        for (size_t i = 0; i < length; i++)
        {
             std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)buffer[i];
        }

        delete buffer;
    }

    fs.close();

    //Gameboy should take the rom as an arg
    Gameboy gameboy = Gameboy();
    gameboy.Run();

    return 0;
}