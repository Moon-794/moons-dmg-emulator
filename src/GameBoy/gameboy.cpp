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
    
    cpu->Step();

    uint32_t cpu_cycles = (cpu->GetCycles() - last_cycles);
    ppu->Step(cpu_cycles);
}

void Gameboy::Reset()
{
    
}

void Gameboy::Quit()
{
     
}