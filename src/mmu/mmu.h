#ifndef GB_MEMORY
#define GB_MEMORY

#define VRAM_START 0x8000
#define VRAM_END 0x9FFF

#include <vector>
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <functional>

#include "joypad/joypad.h"

class Gameboy;

struct Object
{
    uint8_t yPos;
    uint8_t xPos;
    uint8_t tileIndex;
    uint8_t Flags;
};

namespace gb
{  
    class mmu
    {
    friend Joypad;
    friend Gameboy;

    public:
        mmu(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom);

        //returns a byte from the specified 16 bit address
        uint8_t read(uint16_t address);

        //writes a byte to the specified 16 bit address
        void write(uint16_t address, uint8_t data);

        //Print the hexidecimal value at the specified address 
        void PrintByteAsHex(uint16_t address);

        void SetJoypad(Joypad* jp);

    private:
        std::array<uint8_t, 0xFFFF> mem;
        std::vector<uint8_t> bootRom;

        std::vector<std::array<uint8_t, 0x4000>> memory_banks;
        uint8_t active_memory_bank = 1;

        //If true, reads from adresses 0x0000 to 0x00FF will read from bootRom instead of the game cartridge
        //Writing any value to 0xFF50 will effectively unmap the bootrom, and reads will be retrieved from mem
        bool isBootRomMapped = true;

        Joypad* joypad;

        std::function<void()> SerialTransferInitiate;
        
        //Procedure to quickly write 160 bytes of RAM or ROM to OAM
        void DMATransfer(uint16_t address);
        void ChangeMemoryBank(uint8_t bankNo);
    };
}


#endif