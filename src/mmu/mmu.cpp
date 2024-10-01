#include "mmu/mmu.h"

memory::memory()
{
    mem = std::vector<uint8_t>(0xFFFF);
}

uint8_t memory::read(uint16_t address)
{
    if(address > 0xFFFF)
        throw std::out_of_range("Invalid address");

    return mem[address];
}

void memory::write(uint16_t address, uint8_t data)
{
    if(address > 0xFFFF)
        throw std::out_of_range("Invalid address: " + address);
    
    mem[address] = data;
}