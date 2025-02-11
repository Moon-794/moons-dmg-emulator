#include "mmu/mmu.h"
#include <iostream>
#include <iomanip>
#include <bitset>

gb::mmu::mmu(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    this->bootRom = bootRom;

    //Load the first 32KB of memory to mem, comprising of bank 0 and 1
    for (size_t i = 0; i < gameRom.size(); i++)
    {
        if(i < 0x8000)
            mem[i] = gameRom[i];
    }

    uint8_t numBanks = gameRom.size() / 0x4000;
    numBanks--;

    for (size_t i = 0; i < numBanks; i++)
    {
        std::array<uint8_t, 0x4000> bank = {};
        for (size_t f = 0; f < 0x4000; f++)
        {
            bank[f] = gameRom[((i + 1) * 0x4000) + f];
        }

        memory_banks.push_back(bank);
    }
    
}

uint8_t gb::mmu::read(uint16_t address)
{ 
    //Retrieve memory from bootrom if still in the boot sequence
    if(isBootRomMapped && address <= 0x00FF)
        return bootRom[address];
    
    if (address >= 0x4000 && address <= 0x7FFF)
    {
        return memory_banks[active_memory_bank - 1][address - 0x4000];
    }
        

    if (address >= 0xE000 && address <= 0xFDFF)
        return mem[address - 0x2000];

    return mem[address];
}

void gb::mmu::write(uint16_t address, uint8_t data)
{
    if(address > 0xFFFF)
    {
        std::cout << "hello";
        return;
    }

    if(address >= 0x2000 && address <= 0x3FFF)
    {
        ChangeMemoryBank(data);
        return;
    }

    //Dont write to VRAM during mode 3 (pixel drawing)
    if(address >= 0x8000 && address <= 0x9FFF)
    {
        if((mem[0xFF41] & 0b00000011) == 3)
        {
            return;
        }
    }

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
    case 0xFF02:
        if(data == 0x81)
        {
            if(SerialTransferInitiate)
                SerialTransferInitiate();
        }
        mem[address] = data;
        break;
    //DIV Register
    case 0xFF04:
        mem[address] = 0;
        break;
    case 0xFF45:
        mem[address] = data;
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

void gb::mmu::ChangeMemoryBank(uint8_t bankNo)
{
    if(bankNo == 0)
        active_memory_bank = 1;
    else
        active_memory_bank = bankNo;

    //std::cout << (int)active_memory_bank << "\n";
}