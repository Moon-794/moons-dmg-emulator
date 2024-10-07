#include "cpu/cpu.h"
#include <bitset>

//Load the immediate value D16 into the combo register NN
void gb::cpu::LD_NN_D16(InstructionParams* p)
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t value = (msb << 8) | lsb;

    SetComboRegister(p->reg16, value);

    cycles += 12;
}

void gb::cpu::LD_X_YY(uint8_t* x, uint16_t yy)
{
    *x = memory->read(yy);

    cycles += 8;
}

void gb::cpu::LD_A_DE()
{
    LD_X_YY(&a, GetComboRegister(DE));
}

//Load the immediate value D8 into the register x
void gb::cpu::LD_X_D8(InstructionParams* p)
{
    uint8_t value = memory->read(program_counter++);

    *p->reg = value;
    cycles += 8;
}

void gb::cpu::INC_X(uint8_t* reg)
{
    if(((*reg & 0x0F) + (0x01 & 0x0F) & 0x10) == 0x10)
    {
        //bit 3 will carry to bit 4, set half carry flag
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    (*reg)++;

    if(*reg == 0)
        SetFlag(FLAG_Z);
    else
        ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);

    cycles += 4;
}

void gb::cpu::LD_HL_X(uint8_t* reg)
{
    memory->write(GetComboRegister(HL), *reg);
    cycles += 8;
}

//Implementations

//Do nothing
void gb::cpu::NO_OP()
{
    //do nothing!
    cycles += 4;
}

void gb::cpu::LD_A_D8()
{
    InstructionParams p;
    p.reg = &a;

    LD_X_D8(&p);
}

void gb::cpu::LD_C_D8()
{
    InstructionParams p;
    p.reg = &c;

    LD_X_D8(&p);
}

void gb::cpu::LD_DE_D16()
{
    InstructionParams p;
    p.reg16 = DE;

    LD_NN_D16(&p);
}

void gb::cpu::LD_FFC_A()
{
    uint16_t addr = (0xFF00) | c;
    memory->write(addr, a);

    cycles += 8;
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

void gb::cpu::LD_HL_A()
{
    LD_HL_X(&a);
}

//Increment C
void::gb::cpu::INC_C()
{
    INC_X(&c);
}

//Perform a bitwise XOR on the A register with the A register **Note: This always results in a zero
void gb::cpu::XOR_A()
{
    a = a ^ a;
    //Since this always results in a 0, the zero flag is set, the remaining flags are set to 0
    f = f | 0b10000000;

    cycles += 4;
}

void gb::cpu::LDH_A8_A()
{
    uint8_t val = memory->read(program_counter++);

    uint16_t addr = 0xFF00 | val;

    memory->write(addr, a);

    cycles += 12;
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
    instructionTable[0x0C] = &INC_C;
    instructionTable[0x0E] = &LD_C_D8;
    instructionTable[0x11] = &LD_DE_D16;
    instructionTable[0x1A] = &LD_A_DE;
    instructionTable[0x20] = &JR_NZ_D8;
    instructionTable[0x21] = &LD_HL_D16;
    instructionTable[0x31] = &LD_SP_D16;
    instructionTable[0x32] = &LD_HL_DEC_A;
    instructionTable[0x3E] = &LD_A_D8;
    instructionTable[0x77] = &LD_HL_A;
    instructionTable[0xAF] = &XOR_A;

    instructionTable[0xE0] = &LDH_A8_A;
    instructionTable[0xE2] = &LD_FFC_A;

    extendedInstructionTable[0x7C] = &BIT_7_H;
}