#include "gameboy.h"

Gameboy::Gameboy()
{
    mmu = new gb::mmu();
    cpu = new gb::cpu();
    ppu = new gb::ppu();
}

void Gameboy::Run()
{

}

void Gameboy::Reset()
{

}

void Gameboy::Quit()
{
    
}