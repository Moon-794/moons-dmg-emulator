#include "ppu/ppu.h"

void gb::ppu::Step(uint32_t cycles)
{
    clock += cycles;
}