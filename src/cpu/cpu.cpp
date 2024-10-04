#include "cpu/cpu.h"

#include <iostream>
#include <iomanip>

gb::cpu::cpu(gb::mmu* memory)
{
    this->memory = memory;
}

void gb::cpu::Step()
{
    uint8_t instruction = memory->read(program_counter);
    program_counter++;



    std::cout << std::hex << (int)instruction;
}