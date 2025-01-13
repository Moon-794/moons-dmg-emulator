#include "mmu/mmu.h"
#include <iostream>
#include <iomanip>
#include <bitset>

gb::mmu::mmu(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    this->bootRom = bootRom;

    //Load the first 32KB of memory to the game rom
    mem = std::vector<uint8_t>(0xFFFF);
    for (size_t i = 0; i < gameRom.size(); i++)
    {
        mem[i] = gameRom[i];
    }
}

uint8_t gb::mmu::read(uint16_t address)
{ 
    if(address > 0xFFFF)
        throw std::out_of_range("Invalid address");

    //Retrieve memory from bootrom if still in the boot sequence
    if(address <= 0x00FF && isBootRomMapped)
        return bootRom[address];

    return mem[address];
}

void gb::mmu::write(uint16_t address, uint8_t data)
{
    if(address >= 0x2000 && address < 0x4000)
    {
        //Ignore write
        return;
    }

    if(address == 0xFF00)
    {   
        //Lower nibble of 0xFF00 is read only
        uint8_t original = mem[address];
        original &= 0x0F;

        original = (data & 0xF0) | (original & 0x0F);
        mem[address] = original;

        if(joypad)
            joypad->UpdateInputs();

        return;
    }

    if(address == 0xFF46)
    {
        //Initiate DMA Transfer
        DMATransfer(static_cast<uint16_t>(data) << 8);
    }

    if(address == 0xFF50)
    {   
        isBootRomMapped = false;
    }

    mem[address] = data;
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