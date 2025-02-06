#define SFML_STATIC

#ifndef GB_DMG
#define GB_DMG

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "joypad/joypad.h"
#include <chrono>
#include <thread>

class Gameboy
{
public:
    Gameboy(std::vector<uint8_t> bootRom, std::vector<uint8_t> gameRom);

    void SetupLog();

    void Run();
    void Reset();
    void Quit();

private:
    gb::cpu* cpu;
    gb::ppu* ppu;
    gb::mmu* mmu;
    gb::Joypad* joypad;

    std::chrono::high_resolution_clock::time_point divTimer;
};

#endif