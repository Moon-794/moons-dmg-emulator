#include "mmu/mmu.h"

gb::mmu::mmu(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    this->bootRom = bootRom;

    mem = std::vector<uint8_t>(0xFFFF);

    //Load the first 32KB of memory to the game rom
    //Find a more elegent way to do this
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
    if(address < 0x00FF && isBootRomMapped)
        return bootRom[address];        

    return mem[address];
}

void gb::mmu::write(uint16_t address, uint8_t data)
{
    if(address > 0xFFFF)
        throw std::out_of_range("Invalid address: " + address);
    
    //Bootrom map register
    if(address == 0xFF50)
        isBootRomMapped = false;
    
    mem[address] = data;
}