#include "cpu/cpu.h"

#include <iostream>
#include <iomanip>

gb::cpu::cpu(gb::mmu* memory)
{
    this->memory = memory;

    SetupInstructionTable();
}

void gb::cpu::Step()
{
    for (size_t i = 0; i < 2; i++)
    {
        uint8_t instruction = memory->read(program_counter);
        program_counter++;

        (this->*instructionTable[instruction])();
    }
}