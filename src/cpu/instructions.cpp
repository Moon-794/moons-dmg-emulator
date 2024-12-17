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

//Load to the 8 bit register x, the value specified by the 16 bit address YY
void gb::cpu::LD_X_YY(uint8_t* x, uint16_t yy)
{
    *x = memory->read(yy);

    cycles += 8;
}

//Load to the 8 bit register x, the 8 bit value y
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

void gb::cpu::JP_A16()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t newAddr = convert16Bit(lsb, msb);
    program_counter = newAddr;
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

//Do nothing
void gb::cpu::NO_OP()
{
    //do nothing, woo!
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

    cycles += 12;
}

//Load to the address specified by HL, data from A, decrement HL by 1 afterwards
void gb::cpu::LD_HL_DEC_A()
{
    uint16_t addr = GetComboRegister(HL);
    memory->write(addr, a);
    SetComboRegister(HL, addr - 1);
    cycles += 8;
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

void gb::cpu::CALL_A16()
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

void gb::cpu::DEC_XX(RegisterCombo reg)
{
    SetComboRegister(reg, GetComboRegister(reg) - 1);

    cycles += 8;
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

//Relative jump to e if the flag condition CC is true
void gb::cpu::JR_CC_R8(uint8_t flag)
{
    int8_t e = memory->read(program_counter++);
    if(isFlagSet(flag))
    {
        program_counter += e;
        cycles += 12;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::JR_D8()
{
    int8_t e = memory->read(program_counter++);
    program_counter += e;

    cycles += 12;
}

void gb::cpu::LDH_A_A8()
{
    uint16_t addr = 0xFF00;
    uint8_t val = memory->read(program_counter++);

    addr += val;

    a = memory->read(addr);

    cycles += 8;
}

void gb::cpu::SUB_X(uint8_t value)
{
    uint8_t result = a - value;

    if((a & 0x0F) < (value & 0x0F))
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    if(result == 0)
        SetFlag(FLAG_Z);

    SetFlag(FLAG_N);

    a = result;
}

void gb::cpu::CP_HL()
{
    uint8_t data = memory->read(GetComboRegister(HL));
    uint8_t result = a - data;

    if((a & 0x0F) < (data & 0x0F))
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    if(result == 0)
        SetFlag(FLAG_Z);

    SetFlag(FLAG_N);
}

void gb::cpu::ADD_HL()
{
    uint8_t data = memory->read(GetComboRegister(HL));
    uint8_t result = a + data;
    a = result;

    if((a & 0x0F) + (data & 0x0F) > 0x0F)
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    if(result == 0)
        SetFlag(FLAG_Z);

    ResetFlag(FLAG_N);

    if (a + data > 0xFF)
        SetFlag(FLAG_C);
    else
        ResetFlag(FLAG_C);
}

void gb::cpu::DI()
{
    //Disable interrupts
    IME = 0;
    cycles += 4;
}

void gb::cpu::EI()
{
    enable_IME_next_instruction = true;
    cycles += 4;

    std::cout << "yay";
}

void gb::cpu::LD_HL_D8()
{
    uint8_t data = memory->read(program_counter++);
    memory->write(GetComboRegister(HL), data);

    cycles += 12;
}

void gb::cpu::LD_A_HL_INC()
{
    a = memory->read(GetComboRegister(HL));
    SetComboRegister(HL, GetComboRegister(HL) + 1);

    cycles += 8;
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
    instructionTable[0x01] = [this] { gb::cpu::LD_NN_D16(BC); };
    instructionTable[0x04] = [this] { gb::cpu::INC_X(&b); };
    instructionTable[0x05] = [this] { gb::cpu::DEC_X(&b); };
    instructionTable[0x06] = [this] { gb::cpu::LD_X_D8(&b); };
    instructionTable[0x0B] = [this] { gb::cpu::DEC_XX(BC); };
    instructionTable[0x0C] = [this] { gb::cpu::INC_X(&c); };
    instructionTable[0x0D] = [this] { gb::cpu::DEC_X(&c); };
    instructionTable[0x0E] = [this] { gb::cpu::LD_X_D8(&c); };
    instructionTable[0x11] = [this] { gb::cpu::LD_NN_D16(DE); };
    instructionTable[0x13] = [this] { gb::cpu::SetComboRegister(DE, gb::cpu::GetComboRegister(DE) + 1); cycles += 8; };
    instructionTable[0x15] = [this] { gb::cpu::DEC_X(&d); };
    instructionTable[0x16] = [this] { gb::cpu::LD_X_D8(&d); };
    instructionTable[0x17] = [this] { gb::cpu::RL_X(&a); gb::cpu::ResetFlag(FLAG_Z); };
    instructionTable[0x18] = [this] { gb::cpu::JR_D8(); };

    instructionTable[0x1A] = [this] { gb::cpu::LD_X_YY(&a, GetComboRegister(DE)); };
    instructionTable[0x1D] = [this] { gb::cpu::DEC_X(&e); };
    instructionTable[0x1E] = [this] { gb::cpu::LD_X_D8(&e); };
    instructionTable[0x20] = [this] { gb::cpu::JR_NZ_D8(); };
    instructionTable[0x21] = [this] { gb::cpu::LD_NN_D16(HL); };
    instructionTable[0x22] = [this] { gb::cpu::LD_HL_INC_A(); };
    instructionTable[0x23] = [this] { gb::cpu::SetComboRegister(HL, gb::cpu::GetComboRegister(HL) + 1); cycles += 8; };
    instructionTable[0x24] = [this] { gb::cpu::INC_X(&h); };
    instructionTable[0x28] = [this] { gb::cpu::JR_CC_R8(FLAG_Z); };
    instructionTable[0x2A] = [this] { gb::cpu::LD_A_HL_INC(); }; 
    instructionTable[0x2E] = [this] { gb::cpu::LD_X_D8(&l); };
    instructionTable[0x31] = [this] { gb::cpu::LD_SP_D16(); };
    instructionTable[0x32] = [this] { gb::cpu::LD_HL_DEC_A(); };
    instructionTable[0x36] = [this] { gb::cpu::LD_HL_D8(); };
    instructionTable[0x3D] = [this] { gb::cpu::DEC_X(&a); };

    instructionTable[0x3E] = [this] { gb::cpu::LD_X_D8(&a);};
    instructionTable[0x4F] = [this] { gb::cpu::LD_X_Y(&c, a); };
    instructionTable[0x5C] = [this] { gb::cpu::LD_X_Y(&e, h); };
    instructionTable[0x57] = [this] { gb::cpu::LD_X_Y(&d, a); };
    instructionTable[0x67] = [this] { gb::cpu::LD_X_Y(&h, a); };
    instructionTable[0x77] = [this] { gb::cpu::LD_HL_X(&a); };
    instructionTable[0x78] = [this] { gb::cpu::LD_X_Y(&a, b); };
    instructionTable[0x7B] = [this] { gb::cpu::LD_X_Y(&a, e); };
    instructionTable[0x7C] = [this] { gb::cpu::LD_X_Y(&a, h); };
    instructionTable[0x7D] = [this] { gb::cpu::LD_X_Y(&a, l); };
    instructionTable[0x86] = [this] { gb::cpu::ADD_HL(); };
    instructionTable[0x90] = [this] { gb::cpu::SUB_X(b); };
    instructionTable[0xAF] = [this] { gb::cpu::XOR_X(&a); };
    instructionTable[0xBE] = [this] { gb::cpu::CP_HL(); };
    instructionTable[0xC1] = [this] { gb::cpu::POP_XX(BC); };
    instructionTable[0xC3] = [this] { gb::cpu::JP_A16(); };
    instructionTable[0xC5] = [this] { gb::cpu::PUSH_XX(BC); };

    instructionTable[0xC9] = [this] { gb::cpu::RET(); };
    instructionTable[0xCD] = [this] { gb::cpu::CALL_A16(); };
    instructionTable[0xD9] = [this] { gb::cpu::RET(); IME = 1; };
    instructionTable[0xE0] = [this] { gb::cpu::LDH_A8_A(); };
    instructionTable[0xE2] = [this] { gb::cpu::LD_FFC_A(); };
    instructionTable[0xEA] = [this] { gb::cpu::LD_A16_A(); };
    instructionTable[0xF0] = [this] { gb::cpu::LDH_A_A8(); };
    instructionTable[0xF3] = [this] { gb::cpu::DI(); };
    instructionTable[0xFB] = [this] { gb::cpu::EI(); }; 
    instructionTable[0xFE] = [this] { gb::cpu::CP_D8(); }; 

    extendedInstructionTable[0x11] = [this]{ gb::cpu::RL_X(&c); };
    extendedInstructionTable[0x7C] = [this]{ gb::cpu::BIT_N_X(BIT_7, &h); };
}