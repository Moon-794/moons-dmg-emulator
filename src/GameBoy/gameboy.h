#ifndef GB_DMG
#define GB_DMG

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "joypad/joypad.h"
#include <chrono>

class Gameboy
{
public:
    Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom);

    void SetupLog();

    void Run();
    void Reset();
    void Quit();

private:
    std::vector<uint8_t> indices;
    int ticker = 0;

    gb::cpu* cpu;
    gb::ppu* ppu;
    gb::mmu* mmu;
    gb::Joypad* joypad;
    
    //rom file needed too, not sure where to store
};

#endif