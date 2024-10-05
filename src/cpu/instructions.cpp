#include "cpu/cpu.h"
#include <bitset>

void gb::cpu::LD_NN_D16(InstructionParams* p)
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t value = (msb << 8) | lsb;

    SetComboRegister(p->reg16, value);
}

void gb::cpu::NO_OP()
{
    //do nothing!
    cycles += 4;
}

//Load the immediate 16 bit value into the HL register
void gb::cpu::LD_HL_D16()
{
    InstructionParams p;
    p.reg16 = HL;

    LD_NN_D16(&p);

    cycles += 12;
}

//Relative jump IF zero flag is clear (Not Zero)
void gb::cpu::JR_NZ_D8()
{
    int8_t jumpOffset = memory->read(program_counter++);

    if(isFlagSet(FLAG_Z) == false)
    {
        program_counter += jumpOffset;
        cycles += 12;
    }
    else
    {
        cycles += 8;
    }
}

//Load the immediate 16 bit value into the stack pointer register
void gb::cpu::LD_SP_D16()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t value = (msb << 8) | lsb;
    stack_pointer = value;

    cycles += 3;
}

//Load to the address specified by HL, data from A, decrement HL by 1 afterwards
void gb::cpu::LD_HL_DEC_A()
{
    uint16_t addr = GetComboRegister(HL);
    memory->write(addr, a);
    SetComboRegister(HL, addr - 1);
    cycles += 3;
}

//Perform a bitwise XOR on the A register with the A register **Note: This always results in a zero
void gb::cpu::XOR_A()
{
    a = a ^ a;
    //Since this always results in a 0, the zero flag is set, the remaining flags are set to 0
    f = f | 0b10000000;

    cycles += 4;
}

//CB PREFIX TABLE

void gb::cpu::BIT_N_X(InstructionParams* p)
{
    //Test if nth bit of register x is set
    if((*p->reg & p->bit_flag) != 0)
    {
        //Bit is set
        ResetFlag(FLAG_Z);
    }
    else
    {
        //Bit is not set
        SetFlag(FLAG_Z);
    }

    ResetFlag(FLAG_N);
    SetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::BIT_7_H()
{
    InstructionParams p;
    p.reg = &h;
    p.bit_flag = BIT_7;

    BIT_N_X(&p);
}

void gb::cpu::SetupInstructionTables()
{
    instructionTable[0x00] = &NO_OP;
    instructionTable[0x21] = &LD_HL_D16;
    instructionTable[0x20] = &JR_NZ_D8;
    instructionTable[0x31] = &LD_SP_D16;
    instructionTable[0x32] = &LD_HL_DEC_A;
    instructionTable[0xAF] = &XOR_A;

    extendedInstructionTable[0x7C] = &BIT_7_H;
}