#include "gameboy.h"

Gameboy::Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    mmu = new gb::mmu(bootRom, gameRom);
    cpu = new gb::cpu(mmu);
    ppu = new gb::ppu(mmu);
    joypad = new gb::Joypad(mmu);

    mmu->SetJoypad(joypad);
}

void Gameboy::Run()
{
    //Fetch, Decode, Execute cycle, then input, graphics and maybe audio

    uint32_t last_cycles = cpu->GetCycles();

    cpu->Step();

    uint32_t cpu_cycles = (cpu->GetCycles() - last_cycles);
    ppu->Step(cpu_cycles / 4);
}

void Gameboy::SetupLog()
{
    cpu->a = 0x01;
    cpu->f = 0xB0;
    cpu->b = 0x00;
    cpu->c = 0x13;
    cpu->d = 0x00;
    cpu->e = 0xD8;
    cpu->h = 0x01;
    cpu->l = 0x4D;
    cpu->stack_pointer = 0xFFFE;
    cpu->program_counter = 0x100;

    cpu->debug = true;
}

void Gameboy::Reset()
{
    
}

void Gameboy::Quit()
{
    exit(-1);
}