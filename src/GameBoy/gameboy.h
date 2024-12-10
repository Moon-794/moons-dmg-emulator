#ifndef GB_DMG
#define GB_DMG

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"

class Gameboy
{
public:
    Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom);

    void Save(uint32_t saveID);
    void Load(uint32_t saveID);

    void Run();
    void Reset();
    void Quit();

    //Various other functions, not required but definite "nice to have"
    void SaveState();
    void LoadState();

private:
    std::vector<uint8_t> indices;
    int ticker = 0;

    gb::cpu* cpu;
    gb::ppu* ppu;
    gb::mmu* mmu;
    
    //rom file needed too, not sure where to store
};

#endif