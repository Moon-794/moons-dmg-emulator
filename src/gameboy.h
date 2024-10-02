#ifndef GB_DMG
#define GB_DMG

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"

class Gameboy
{
public:
    Gameboy();

    void Run();
    void Reset();
    void Quit();

    //Various other functions, not required but definite nice to haves
    void SaveState();
    void LoadState();

private:
    gb::cpu* cpu;
    gb::ppu* ppu;
    gb::mmu* mmu;
    //rom file needed too, not sure where to strore 
};

#endif