#include "gameboy.h"

Gameboy::Gameboy()
{
    mmu = new gb::mmu();
    cpu = new gb::cpu();
    ppu = new gb::ppu();
}

void Gameboy::Run()
{
    //Fetch, Decode, Execute cycle
}

void Gameboy::Reset()
{

}

void Gameboy::Quit()
{
    
}