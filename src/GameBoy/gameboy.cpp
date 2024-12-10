#include "gameboy.h"

Gameboy::Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    mmu = new gb::mmu(bootRom, gameRom);
    cpu = new gb::cpu(mmu);
    ppu = new gb::ppu(mmu);
}

void Gameboy::Run()
{
    //Fetch, Decode, Execute cycle, then graphics and maybe audio

    uint32_t last_cycles = cpu->GetCycles();
    
    int result = cpu->Step();

    if(result == -1)
    {
        Quit();
    }

    uint32_t cpu_cycles = (cpu->GetCycles() - last_cycles);
    ppu->Step(cpu_cycles);

    if(ticker == 0)
    {
        indices.clear();
        for (size_t i = 0; i < 1024; i++)
        {
            uint8_t index = mmu->read(0x9800 + i);
            indices.push_back(index);
        }

        ticker = 6000;
        ppu->window.DrawTile(indices[260]);
    }
    else
    {
        ticker--;
    }

    
}

void Gameboy::Reset()
{
    
}

void Gameboy::Quit()
{
    exit(-1);
}