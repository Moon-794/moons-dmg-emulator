#include "cpu/cpu.h"

void gb::cpu::LD_SP_NN()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t value = (msb << 8) | lsb;
    stack_pointer = value;

    cycles += 3;
}

void gb::cpu::XOR_A()
{
    a = a ^ a;
    //Since this always results in a 0, the zero flag is set
    f = f | 0b1000000;

    cycles += 4;
}

void gb::cpu::SetupInstructionTable()
{
    instructionTable[0x31] = &LD_SP_NN;
    instructionTable[0xAF] = &XOR_A;
}