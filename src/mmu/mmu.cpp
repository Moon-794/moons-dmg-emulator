#include "mmu/mmu.h"
#include <iostream>
#include <iomanip>
#include <bitset>

gb::mmu::mmu(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    this->bootRom = bootRom;

    //Load the first 32KB of memory to the game rom
    mem = {};
    for (size_t i = 0; i < gameRom.size(); i++)
    {
        mem[i] = gameRom[i];
    }
}

uint8_t gb::mmu::read(uint16_t address)
{ 
    //Retrieve memory from bootrom if still in the boot sequence
    if(isBootRomMapped && address <= 0x00FF)
        return bootRom[address];

    //if(address == 0xff44)
       //return 0x90;

    return mem[address];
}

void gb::mmu::write(uint16_t address, uint8_t data)
{
    switch (address) 
    {
    case 0xFF00:
        // Handle joypad register
        {
            uint8_t original = mem[address];
            original &= 0x0F;
            original = (data & 0xF0) | (original & 0x0F);
            mem[address] = original;

            if(joypad)
                joypad->UpdateInputs();
        }
        break;
    case 0xFF46:
        DMATransfer(static_cast<uint16_t>(data) << 8);
        break;
    case 0xFF50:
        isBootRomMapped = false;
        break;
    default:
        if (address >= 0x2000 && address < 0x4000) {
            // Ignore write
            return;
        }
        mem[address] = data;
        break;
    }
}

void::gb::mmu::PrintByteAsHex(uint16_t address)
{
    std::cout << 
    std::hex << 
    std::uppercase << 
    std::setw(2) << 
    std::setfill('0') << (int)read(address) << " at address: " << (int)address << "\n";
}

void gb::mmu::SetJoypad(Joypad* jp)
{
    joypad = jp;
}

void gb::mmu::DMATransfer(uint16_t addr)
{
    for (size_t i = 0; i < 0xA0; i++)
    {
        write(0xFE00 + i, read(addr + i));
    }
}