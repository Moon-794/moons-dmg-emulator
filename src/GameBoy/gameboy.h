#define SFML_STATIC

#ifndef GB_DMG
#define GB_DMG

#define CLOCK_SPEED 4194304

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "serial/serial.h"
#include "joypad/joypad.h"
#include <unistd.h>
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
    gb::serial* serial;

    void ProcessDIV();
    void ProcessSerial(uint8_t last_cycles);

    std::chrono::high_resolution_clock::time_point divTimer;
};

#endif