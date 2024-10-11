#include "cpu/cpu.h"
#include <bitset>

//Load the immediate value D16 into the combo register NN
void gb::cpu::LD_NN_D16(RegisterCombo reg16)
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t value = (msb << 8) | lsb;

    SetComboRegister(reg16, value);

    cycles += 12;
}

void gb::cpu::LD_X_YY(uint8_t* x, uint16_t yy)
{
    *x = memory->read(yy);

    cycles += 8;
}

void gb::cpu:: LD_X_Y(uint8_t* x, uint8_t y)
{
    *x = y;

    cycles += 4;
}

//Load the immediate value D8 into the register x
void gb::cpu::LD_X_D8(uint8_t* reg)
{
    uint8_t value = memory->read(program_counter++);

    *reg = value;
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

void gb::cpu::LD_FFC_A()
{
    uint16_t addr = (0xFF00) | c;
    memory->write(addr, a);

    cycles += 8;
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

void gb::cpu::LD_HL_INC_A()
{
    memory->write(GetComboRegister(HL), a);
    
    uint16_t newHL = GetComboRegister(HL) + 1;
    SetComboRegister(HL, newHL);

    cycles += 8;
}

void gb::cpu::LD_HL_A()
{
    LD_HL_X(&a);
}

void gb::cpu::XOR_X(uint8_t* reg)
{
    *reg = *reg ^ a;

    if(*reg == 0)
    {
        SetFlag(FLAG_Z);
    }
    else
    {
        ResetFlag(FLAG_Z);
    }

    SetFlag(FLAG_N);
    SetFlag(FLAG_H);
    SetFlag(FLAG_C);

    cycles += 4;
}

void gb::cpu::LDH_A8_A()
{
    uint8_t val = memory->read(program_counter++);

    uint16_t addr = 0xFF00 | val;

    memory->write(addr, a);

    cycles += 12;
}

void gb::cpu::CD()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t addr = convert16Bit(lsb, msb);

    uint8_t pcMSB = program_counter >> 8;
    uint8_t pcLSB = program_counter & 0x00FF;

    stack_pointer--;
    memory->write(stack_pointer, pcMSB);
    stack_pointer--;
    memory->write(stack_pointer, pcLSB);

    program_counter = addr;
    cycles += 24;
}

void gb::cpu::PUSH_XX(RegisterCombo reg)
{
    uint16_t value = GetComboRegister(reg);

    uint8_t highByte = value >> 8;
    uint8_t lowByte = value & 0xFF;

    stack_pointer--;
    memory->write(stack_pointer, highByte);
    stack_pointer--;
    memory->write(stack_pointer, lowByte);

    cycles += 16;
}

void gb::cpu::POP_XX(RegisterCombo reg)
{
    uint8_t lsb = memory->read(stack_pointer++);
    uint8_t msb = memory->read(stack_pointer++);

    uint16_t data = (msb << 8) | lsb;

    SetComboRegister(reg, data);

    cycles += 12;
}

void gb::cpu::DEC_X(uint8_t* reg)
{
    if((*reg & 0x0F) == 0x00)
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    *reg = *reg - 1;

    if(*reg == 0x00)
    {
        SetFlag(FLAG_Z);
    }
    else
    {
        ResetFlag(FLAG_Z);
    }

    SetFlag(FLAG_N);

    cycles += 4;
}

void gb::cpu::RET()
{
    uint8_t lsb = memory->read(stack_pointer++);
    uint8_t msb = memory->read(stack_pointer++);

    program_counter = convert16Bit(lsb, msb);

    cycles += 16;
}

//Subtract D8 from the A register, without storing it and set flags
void gb::cpu::CP_D8()
{
    uint8_t value = memory->read(program_counter++);
    uint8_t result = a - value;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    a < value ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    (a & 0x0F) < (value & 0x0F) ? SetFlag(FLAG_H) : ResetFlag(FLAG_H); 

    SetFlag(FLAG_N);

    cycles += 8;
}

void gb::cpu::LD_A16_A()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t addr = convert16Bit(lsb, msb);

    memory->write(addr, a);

    cycles += 16;
}

//CB PREFIX TABLE

void gb::cpu::RL_X(uint8_t* reg)
{
    uint8_t newCarry = (*reg & BIT_7) >> 7;
    *reg = *reg << 1 | (f & BIT_4 ? 1 : 0);

    if(*reg == 0)
    {
        SetFlag(FLAG_Z);
    }
    else
    {
        ResetFlag(FLAG_Z);
    }

    ResetFlag(FLAG_H);
    ResetFlag(FLAG_N);

    if(newCarry == 1)
    {
        SetFlag(FLAG_C);
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    cycles += 8;
}

void gb::cpu::BIT_N_X(uint8_t n, uint8_t* reg)
{
    //Test if nth bit of register x is set
    if((*reg & n) != 0)
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

void gb::cpu::SetupInstructionTables()
{
    instructionTable[0x00] = [this] { gb::cpu::NO_OP(); };
    instructionTable[0x05] = [this] { gb::cpu::DEC_X(&b); };
    instructionTable[0x06] = [this] { gb::cpu::LD_X_D8(&b); };
    instructionTable[0x0C] = [this] { gb::cpu::INC_X(&c); };
    instructionTable[0x0E] = [this] { gb::cpu::LD_X_D8(&c); };
    instructionTable[0x11] = [this] { gb::cpu::LD_NN_D16(DE); };
    instructionTable[0x13] = [this] { gb::cpu::SetComboRegister(DE, gb::cpu::GetComboRegister(DE) + 1); cycles += 8; };
    instructionTable[0x17] = [this] { gb::cpu::RL_X(&a); gb::cpu::ResetFlag(FLAG_Z); };
    instructionTable[0x1A] = [this] { gb::cpu::LD_X_YY(&a, GetComboRegister(DE)); };
    instructionTable[0x20] = [this] { gb::cpu::JR_NZ_D8(); };
    instructionTable[0x21] = [this] { gb::cpu::LD_NN_D16(HL); };
    instructionTable[0x22] = [this] { gb::cpu::LD_HL_INC_A(); };
    instructionTable[0x23] = [this] { gb::cpu::SetComboRegister(HL, gb::cpu::GetComboRegister(HL) + 1); cycles += 8; };
    instructionTable[0x31] = [this] { gb::cpu::LD_SP_D16(); };
    
    instructionTable[0x32] = [this] { gb::cpu::LD_HL_DEC_A(); };
    instructionTable[0x3D] = [this] { gb::cpu::DEC_X(&a); };
    instructionTable[0x3E] = [this] { gb::cpu::LD_X_D8(&a); };
    instructionTable[0x5C] = [this] { gb::cpu::LD_X_Y(&e, h); };
    instructionTable[0x77] = [this] { gb::cpu::LD_HL_X(&a); };
    instructionTable[0x7B] = [this] { gb::cpu::LD_X_Y(&a, e); };
    instructionTable[0xAF] = [this] { gb::cpu::XOR_X(&a); };

    instructionTable[0x4F] = [this] { gb::cpu::LD_X_Y(&c, a); };
    instructionTable[0xC1] = [this] { gb::cpu::POP_XX(BC); };
    instructionTable[0xC5] = [this] { gb::cpu::PUSH_XX(BC); };
    instructionTable[0xC9] = [this] { gb::cpu::RET(); };
    instructionTable[0xCD] = [this] { gb::cpu::CD(); };
    instructionTable[0xE0] = [this] { gb::cpu::LDH_A8_A(); };
    instructionTable[0xE2] = [this] { gb::cpu::LD_FFC_A(); };
    instructionTable[0xEA] = [this] { gb::cpu::LD_A16_A(); };
    instructionTable[0xFE] = [this] { gb::cpu::CP_D8(); };

    extendedInstructionTable[0x11] = [this]{ gb::cpu::RL_X(&c); };
    extendedInstructionTable[0x7C] = [this]{ gb::cpu::BIT_N_X(BIT_7, &h); };
}