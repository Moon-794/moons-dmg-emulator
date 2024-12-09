#include <iostream>
#include <fstream>
#include <iomanip>

#include "GameBoy/gameboy.h"

std::vector<uint8_t> GetRomContentsFromFile(char* path);

int main(int argc, char** args)
{
    //Argument checking
    if(argc < 3)
    {
        std::cout << "Error: Pass a BootROM file and a game ROM file as the first and second argument" << std::endl;
        return 1;
    }

    std::vector<uint8_t> bootRom = GetRomContentsFromFile(args[1]);
    std::vector<uint8_t> gameRom = GetRomContentsFromFile(args[2]);

    //Validate file sizes, bootrom must ALWAYS be 256 bytes
    if(bootRom.size() != 256 || gameRom.size() == 0)
    { 
        std::cout << "Error reading from ROM files" << std::endl;
        return 1;
    }

    std::cout << "Bootrom size: " << bootRom.size() << std::endl;
    std::cout << "Gamerom size: " << gameRom.size() << std::endl;

    //Pass control to the emulator
    Gameboy gameboy = Gameboy(bootRom, gameRom);

    while(true)
        gameboy.Run();

    return 0;
}

std::vector<uint8_t> GetRomContentsFromFile(char* path)
{
    std::vector<uint8_t> romData;
    std::ifstream fs(path, std::ios::in | std::ios::binary);

    if(!fs)
    {
        std::cout << "Failed to open file:" << path << std::endl;
        return romData; //return empty vector
    }
    else
    {
        //Read entire contents of file
        fs.seekg (0, fs.end);
        int length = fs.tellg();
        fs.seekg (0, fs.beg);

        unsigned char* buffer = new unsigned char[length];
        fs.read((char*)buffer, length);

        romData = std::vector<uint8_t>(length);

        //Pass data into vector in a usable format
        for (size_t i = 0; i < length; i++)
        {
            romData[i] = (int8_t)buffer[i];
        }
        
        delete buffer;
    }

    fs.close();

    return romData;
}