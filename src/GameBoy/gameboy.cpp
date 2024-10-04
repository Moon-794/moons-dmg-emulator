#include "gameboy.h"

Gameboy::Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom)
{
    mmu = new gb::mmu(bootRom, gameRom);
    cpu = new gb::cpu(mmu);
    ppu = new gb::ppu();
}

void Gameboy::Run()
{
    //Fetch, Decode, Execute cycle, then graphics and maybe audio
    cpu->Step();
}

void Gameboy::Reset()
{

}

void Gameboy::Quit()
{
     
}