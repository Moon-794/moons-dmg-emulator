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
    uint8_t result = (*reg) ^ a;

    a = result;

    if(result == 0)
    {
        SetFlag(FLAG_Z);
    }
    else
    {
        ResetFlag(FLAG_Z);
    }

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);
    ResetFlag(FLAG_C);

    cycles += 4;
}

void gb::cpu::LDH_A8_A()
{
    uint8_t val = memory->read(program_counter++);

    uint16_t addr = 0xFF00 | val;
    memory->write(addr, a);

    if(val == 0xE1)
        std::cout << "EOA: " << (int)a << "\n";

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

    --(*reg);

    if((*reg) == 0x00)
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

void gb::cpu::JR_D8()
{
    int8_t e = memory->read(program_counter++);
    program_counter += e;

    cycles += 12;
}

void gb::cpu::LDH_A_A8()
{
    uint8_t val = memory->read(program_counter++);
    uint16_t addr = convert16Bit(val, 0xFF);

    a = memory->read(addr);

    cycles += 12;
}

void gb::cpu::SUB_X(uint8_t value)
{
    uint8_t result = a - value;

    if(a < value)
    {
        SetFlag(FLAG_C);
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    if((a & 0x0F) < (value & 0x0F))
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    SetFlag(FLAG_N);

    a = result;

    cycles += 4;
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

    cycles += 8;
}

void gb::cpu::CP_X(uint8_t value)
{
    uint8_t result = a - value;

    if(a < value)
    {
        SetFlag(FLAG_C);
    }
    else
    {
        ResetFlag(FLAG_C);
    }

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

    cycles += 4;
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

void gb::cpu::OR_X(uint8_t* reg)
{
    uint8_t result = a | *reg;
    a = result;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_H);
}

void gb::cpu::AND_X(uint8_t* reg)
{
    uint8_t result = a & *reg;
    a = result;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    SetFlag(FLAG_H);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_N);

    cycles += 4;
}

void gb::cpu::RET_NZ()
{
    if(!isFlagSet(FLAG_Z))
    {
        uint8_t lsb = memory->read(stack_pointer++);
        uint8_t msb = memory->read(stack_pointer++);

        program_counter = convert16Bit(lsb, msb);

        cycles += 20;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::RET_C()
{
    if(isFlagSet(FLAG_C))
    {
        uint8_t lsb = memory->read(stack_pointer++);
        uint8_t msb = memory->read(stack_pointer++);

        program_counter = convert16Bit(lsb, msb);

        cycles += 20;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::LD_A_NN()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    uint16_t addr = convert16Bit(lsb, msb);
    a = memory->read(addr);

    cycles += 16;
}

void gb::cpu::RET_Z()
{
    if(isFlagSet(FLAG_Z))
    {
        uint8_t lsb = memory->read(stack_pointer++);
        uint8_t msb = memory->read(stack_pointer++);

        program_counter = convert16Bit(lsb, msb);

        cycles += 20;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::LD_X_HL(uint8_t* reg)
{
    uint16_t addr = GetComboRegister(HL);
    *reg = memory->read(addr);

    cycles += 8;
}

void gb::cpu::INC_XX_VAL(RegisterCombo reg)
{
    uint16_t regValue = GetComboRegister(reg);
    uint8_t byte = memory->read(regValue);

    byte++;
    memory->write(regValue, byte);

    byte == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    (byte & 0x0F) == 0 ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    ResetFlag(FLAG_N);

    cycles += 12;
}

void gb::cpu::CPL()
{
    a = ~a;

    SetFlag(FLAG_N);
    SetFlag(FLAG_H);

    cycles += 4;
}

void gb::cpu::AND_N()
{
    uint8_t data = memory->read(program_counter++);
    a = a & data;

    a == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    
    SetFlag(FLAG_H);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_C);

    cycles += 8;
}

void gb::cpu::RST_XX(uint16_t address)
{
    uint8_t pcMSB = program_counter >> 8;
    uint8_t pcLSB = program_counter & 0x00FF;

    stack_pointer--;
    memory->write(stack_pointer, pcMSB);
    stack_pointer--;
    memory->write(stack_pointer, pcLSB);

    program_counter = convert16Bit(address, 0x00);

    cycles += 16;
}

void gb::cpu::ADD_X(uint8_t* reg)
{
    uint16_t result = a + (*reg);

    (result & 0xFF) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);

    ((a & 0x0F) + ((*reg) & 0x0F)) > 0x0F ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    result > 0xFF ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
}

void gb::cpu::ADD_N()
{
    uint8_t data = memory->read(program_counter++);

    uint16_t result = a + data;

    (result & 0xFF) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);

    ((a & 0x0F) + (data & 0x0F)) > 0x0F ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    result > 0xFF ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);

    a = result & 0xFF;
}

void gb::cpu::ADD_XX_YY(RegisterCombo first, RegisterCombo second)
{
    uint16_t firstVal = GetComboRegister(first);
    uint16_t secondVal = GetComboRegister(second);

    uint32_t result = firstVal + secondVal;

    SetComboRegister(first, result);

    ((firstVal & 0x0FFF) + (secondVal & 0x0FFF) > 0x0FFF) ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    result > 0xFFFF ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    ResetFlag(FLAG_N);

    cycles += 8;
}

void gb::cpu::LD_X_DHL(uint8_t* reg)
{
    *reg = memory->read(GetComboRegister(HL));

    cycles += 8;
}

void gb::cpu::JP_HL()
{
    program_counter = GetComboRegister(HL);
    cycles += 4;
}

void gb::cpu::LD_XX_Y(RegisterCombo regCombo, uint8_t* reg)
{
    memory->write(GetComboRegister(regCombo), *reg);
    cycles += 8;
}

void gb::cpu::CALL_NZ_NN()
{
    uint8_t addrLSB = memory->read(program_counter++);
    uint8_t addrMSB = memory->read(program_counter++);

    uint16_t addr = convert16Bit(addrLSB, addrMSB);

    if(!isFlagSet(FLAG_Z))
    {
        uint8_t pcMSB = program_counter >> 8;
        uint8_t pcLSB = program_counter & 0x00FF;

        stack_pointer--;
        memory->write(stack_pointer, pcMSB);
        stack_pointer--;
        memory->write(stack_pointer, pcLSB);

        program_counter = addr;

        cycles += 24;
    }
    else
    {
        cycles += 12;
    }
}

void gb::cpu::RLCA()
{
    uint8_t leftMostBit = (a & BIT_7) >> 7;

    a = (a << 1) | leftMostBit;

    leftMostBit != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);

    ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 4;
}

void gb::cpu::RRCA()
{
    uint8_t rightMostBit = (a & BIT_0);

    a = (a >> 1) | (rightMostBit << 7);

    rightMostBit != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);

    ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 4;
}

void gb::cpu::ADC_X(uint8_t* reg)
{
    uint8_t carry;
    isFlagSet(FLAG_C) ? carry = 1 : carry = 0;

    uint16_t result = a + *reg;
    result += carry;

    (result & 0xFF) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);

    ((a & 0x0F) + ((*reg) & 0x0F) + carry) > 0x0F ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    result > 0xFF ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);

    a = static_cast<uint8_t>(result & 0xFF);

    cycles += 4;
}

//Relative jump based on condition cc
void gb::cpu::JR_CC_E(bool condition)
{
    int8_t offset = memory->read(program_counter++);

    if(condition)
    {
        program_counter += offset;
        cycles += 12;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::LD_A_HL_DEC()
{
    a = memory->read(GetComboRegister(HL));

    SetComboRegister(HL, GetComboRegister(HL) - 1);

    cycles += 8;
}

void gb::cpu::JP_CC_NN(bool condition)
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);

    if(condition)
    {
        program_counter = convert16Bit(lsb, msb);
        cycles += 16;
    }
    else
    {
        cycles += 12;
    }
}

void gb::cpu::DEC_I_XX(RegisterCombo reg)
{
    uint8_t val = memory->read(GetComboRegister(reg));
    val--;

    memory->write(GetComboRegister(reg), val);

    val == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    (val & 0x0F) == 0x0F ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);
    SetFlag(FLAG_N);

    cycles += 12;
}

void gb::cpu::RRA()
{
    uint8_t rightMostBit = (a & BIT_0);
    uint8_t carryBit = (isFlagSet(FLAG_C) ? 1 : 0);

    a = (a >> 1) | (carryBit << 7); 

    if (rightMostBit != 0) {
        SetFlag(FLAG_C);
    } else {
        ResetFlag(FLAG_C);
    }

    ResetFlag(FLAG_Z); 
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H); 

    cycles += 4;
}

void gb::cpu::ADC_A_NN(uint8_t val)
{
    uint8_t carryVal = isFlagSet(FLAG_C) ? 1 : 0;
    uint16_t result = a + carryVal + val;

    ((result & 0xFF) == 0) ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    result > 0xFF ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    ((a & 0xF) + (val & 0xF) + carryVal) > 0xF ? SetFlag(FLAG_H) : ResetFlag(FLAG_H);

    ResetFlag(FLAG_N);

    a = result & 0xFF;

    cycles += 8;
}

void gb::cpu::RET_NC()
{
    if(!isFlagSet(FLAG_C))
    {
        uint8_t lsb = memory->read(stack_pointer++);
        uint8_t msb = memory->read(stack_pointer++);

        program_counter = convert16Bit(lsb, msb);

        cycles += 20;
    }
    else
    {
        cycles += 8;
    }
}

void gb::cpu::XOR_HL()
{
    uint8_t val = memory->read(GetComboRegister(HL));
    uint8_t result = a ^ val;

    a = result;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::XOR_A_D8()
{
    uint8_t val = memory->read(program_counter++);

    a = a ^ val;

    a != 0 ? ResetFlag(FLAG_Z) : SetFlag(FLAG_Z);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_H);
    ResetFlag(FLAG_N);

    cycles += 8;
}

void gb::cpu::OR_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    uint8_t result = a | value;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    a = result;
    cycles += 8;
}

void gb::cpu::DAA()
{
    
}

void gb::cpu::SBC_D8()
{
    uint8_t data = memory->read(program_counter++);
    uint8_t carry = isFlagSet(FLAG_C) ? 1 : 0;

    uint8_t result = a - carry - data;

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    if(a < (data + carry))
    {
        SetFlag(FLAG_C);
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    if((a & 0x0F) < ((data) & 0x0F) + (carry & 0x0F))
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    SetFlag(FLAG_N);

    a = result;

    cycles += 8;
}

//Performs a bitwise OR between the A register and the immediate data D8, the result is stored back in the A register
void gb::cpu::OR_D8()
{
    uint8_t value = memory->read(program_counter++);
    uint8_t result = a | value;

    if(result == 0)
    {
        SetFlag(FLAG_Z);
    }
    else
    {
        ResetFlag(FLAG_Z);
    }

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);
    ResetFlag(FLAG_C);

    a = result;

    cycles += 8;
}

void gb::cpu::LD_D16_SP()
{
    uint8_t lsb = memory->read(program_counter++);
    uint8_t msb = memory->read(program_counter++);
    uint16_t addr = convert16Bit(lsb, msb);
    
    uint8_t spLSB = (stack_pointer & 0x00FF);
    uint8_t spMSB = (stack_pointer & 0xFF00) >> 8;

    memory->write(addr, spLSB);
    memory->write(addr + 1, spMSB);

    cycles += 20;
}

void gb::cpu::LD_SP_HL()
{
    stack_pointer = GetComboRegister(HL);
    cycles += 8;
}

void gb::cpu::SCF()
{
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);
    SetFlag(FLAG_C);

    cycles += 4;
}

void gb::cpu::CCF()
{
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    if(isFlagSet(FLAG_C))
    {
        ResetFlag(FLAG_C);
    }
    else
    {
        SetFlag(FLAG_C);
    }

    cycles += 4;
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

//Test if the nth bit of register x is set
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

void gb::cpu::SWAP_X(uint8_t* reg)
{
    uint8_t lower = ((*reg) & 0xF0) >> 4;
    uint8_t higher = ((*reg) & 0x0F) << 4;

    uint8_t newValue = higher | lower;

    *reg = newValue;
    *reg == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_C);
    ResetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::RES_N_X(uint8_t* reg, uint8_t bitNo)
{
    *reg &= ~bitNo;

    cycles += 8;
}

void gb::cpu::RES_N_HL(uint8_t bitNo)
{
    uint8_t val = memory->read(GetComboRegister(HL));

    val &= ~bitNo;

    memory->write(GetComboRegister(HL), val);

    cycles += 16;
}

void gb::cpu::SET_N_HL(uint8_t bitNo)
{
    uint8_t val = memory->read(GetComboRegister(HL));

    val |= bitNo;

    memory->write(GetComboRegister(HL), val);

    cycles += 16;
}

void gb::cpu::SLA_X(uint8_t* reg)
{
    *reg = ((*reg) << 1) & 0xFF;

    ((*reg) & BIT_7) != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    (*reg) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::BIT_N_HL(uint8_t bitNo)
{
    uint8_t val = memory->read(GetComboRegister(HL));

    (val & bitNo) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);
    SetFlag(FLAG_H);

    cycles += 12;
}

//Shifts the register 1 bit to the right, MSB is set to 0, LSB shifted into carry flag
void gb::cpu::SRL_X(uint8_t* reg)
{
    uint8_t lsb = (*reg) & BIT_0;
    (*reg) = (*reg) >> 1;

    *reg == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    lsb == 0 ? ResetFlag(FLAG_C) : SetFlag(FLAG_C);
    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::RR_X(uint8_t* reg)
{
    uint8_t lsb = (*reg) & BIT_0;
    uint8_t carry = isFlagSet(FLAG_C) ? 1 : 0;

    *reg = (*reg) >> 1;
    
    if(carry == 1)
        *reg |= BIT_7;

    lsb != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    (*reg) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    cycles += 8;
}

void gb::cpu::SetupInstructionTables()
{
    instructionTable[0x00] = [this] { gb::cpu::NO_OP(); };
    instructionTable[0x01] = [this] { gb::cpu::LD_NN_D16(BC); };
    instructionTable[0x03] = [this] { gb::cpu::SetComboRegister(BC, gb::cpu::GetComboRegister(BC) + 1); cycles += 8; };
    instructionTable[0x04] = [this] { gb::cpu::INC_X(&b); };
    instructionTable[0x05] = [this] { gb::cpu::DEC_X(&b); };
    instructionTable[0x06] = [this] { gb::cpu::LD_X_D8(&b); };
    instructionTable[0x07] = [this] { gb::cpu::RLCA(); };
    instructionTable[0x08] = [this] { gb::cpu::LD_D16_SP(); };
    instructionTable[0x09] = [this] { gb::cpu::ADD_XX_YY(HL, BC); };
    instructionTable[0x0A] = [this] { gb::cpu::LD_X_YY(&a, GetComboRegister(BC)); };
    instructionTable[0x0B] = [this] { gb::cpu::DEC_XX(BC); };
    instructionTable[0x0C] = [this] { gb::cpu::INC_X(&c); };
    instructionTable[0x0D] = [this] { gb::cpu::DEC_X(&c); };
    instructionTable[0x0E] = [this] { gb::cpu::LD_X_D8(&c); };
    instructionTable[0x0F] = [this] { gb::cpu::RLCA(); };

    instructionTable[0x11] = [this] { gb::cpu::LD_NN_D16(DE); };
    instructionTable[0x12] = [this] { gb::cpu::LD_XX_Y(DE, &a); };
    instructionTable[0x13] = [this] { gb::cpu::SetComboRegister(DE, gb::cpu::GetComboRegister(DE) + 1); cycles += 8; };
    instructionTable[0x14] = [this] { gb::cpu::INC_X(&d); };
    instructionTable[0x15] = [this] { gb::cpu::DEC_X(&d); };
    instructionTable[0x16] = [this] { gb::cpu::LD_X_D8(&d); };
    instructionTable[0x17] = [this] { gb::cpu::RL_X(&a); gb::cpu::ResetFlag(FLAG_Z); };
    instructionTable[0x18] = [this] { gb::cpu::JR_D8(); };
    instructionTable[0x19] = [this] { gb::cpu::ADD_XX_YY(HL, DE); };
    instructionTable[0x1A] = [this] { gb::cpu::LD_X_YY(&a, GetComboRegister(DE)); };
    instructionTable[0x1B] = [this] { gb::cpu::DEC_XX(DE); };
    instructionTable[0x1C] = [this] { gb::cpu::INC_X(&e); };
    instructionTable[0x1D] = [this] { gb::cpu::DEC_X(&e); };
    instructionTable[0x1E] = [this] { gb::cpu::LD_X_D8(&e); };
    instructionTable[0x1F] = [this] { gb::cpu::RRA(); };

    instructionTable[0x20] = [this] { gb::cpu::JR_CC_E(!isFlagSet(FLAG_Z)); };
    instructionTable[0x21] = [this] { gb::cpu::LD_NN_D16(HL); };
    instructionTable[0x22] = [this] { gb::cpu::LD_HL_INC_A(); };
    instructionTable[0x23] = [this] { gb::cpu::SetComboRegister(HL, gb::cpu::GetComboRegister(HL) + 1); cycles += 8; };
    instructionTable[0x24] = [this] { gb::cpu::INC_X(&h); };
    instructionTable[0x25] = [this] { gb::cpu::DEC_X(&h); };
    instructionTable[0x26] = [this] { gb::cpu::LD_X_D8(&h); };
    instructionTable[0x28] = [this] { gb::cpu::JR_CC_E(isFlagSet(FLAG_Z)); };
    instructionTable[0x29] = [this] { gb::cpu::ADD_XX_YY(HL, HL); };
    instructionTable[0x2A] = [this] { gb::cpu::LD_A_HL_INC(); };
    instructionTable[0x2B] = [this] { gb::cpu::DEC_XX(HL); }; 
    instructionTable[0x2C] = [this] { gb::cpu::INC_X(&l); };
    instructionTable[0x2D] = [this] { gb::cpu::DEC_X(&l); };
    instructionTable[0x2E] = [this] { gb::cpu::LD_X_D8(&l); };
    instructionTable[0x2F] = [this] { gb::cpu::CPL(); };

    instructionTable[0x30] = [this] { gb::cpu::JR_CC_E(!isFlagSet(FLAG_C)); };
    instructionTable[0x31] = [this] { gb::cpu::LD_SP_D16(); };
    instructionTable[0x32] = [this] { gb::cpu::LD_HL_DEC_A(); };
    instructionTable[0x33] = [this] { stack_pointer++; cycles += 8; };
    instructionTable[0x34] = [this] { gb::cpu::INC_XX_VAL(HL); };
    instructionTable[0x35] = [this] { gb::cpu::DEC_I_XX(HL); };
    instructionTable[0x36] = [this] { gb::cpu::LD_HL_D8(); };
    instructionTable[0x37] = [this] { gb::cpu::SCF(); };
    instructionTable[0x38] = [this] { gb::cpu::JR_CC_E(isFlagSet(FLAG_C)); };
    instructionTable[0x3A] = [this] { gb::cpu::LD_A_HL_DEC(); };
    instructionTable[0x3B] = [this] { stack_pointer--; cycles += 8; };
    instructionTable[0x3C] = [this] { gb::cpu::INC_X(&a); };
    instructionTable[0x3D] = [this] { gb::cpu::DEC_X(&a); };
    instructionTable[0x3E] = [this] { gb::cpu::LD_X_D8(&a); };
    instructionTable[0x3F] = [this] { gb::cpu::CCF(); };

    instructionTable[0x40] = [this] { gb::cpu::LD_X_Y(&b, b); };
    instructionTable[0x41] = [this] { gb::cpu::LD_X_Y(&b, c); };
    instructionTable[0x42] = [this] { gb::cpu::LD_X_Y(&b, d); };
    instructionTable[0x43] = [this] { gb::cpu::LD_X_Y(&b, e); };
    instructionTable[0x44] = [this] { gb::cpu::LD_X_Y(&b, h); };
    instructionTable[0x45] = [this] { gb::cpu::LD_X_Y(&b, l); };
    instructionTable[0x46] = [this] { gb::cpu::LD_X_DHL(&b); };
    instructionTable[0x47] = [this] { gb::cpu::LD_X_Y(&b, a); };
    instructionTable[0x48] = [this] { gb::cpu::LD_X_Y(&c, b); };
    instructionTable[0x49] = [this] { gb::cpu::LD_X_Y(&c, c); };
    instructionTable[0x4A] = [this] { gb::cpu::LD_X_Y(&c, d); };
    instructionTable[0x4B] = [this] { gb::cpu::LD_X_Y(&c, e); };
    instructionTable[0x4C] = [this] { gb::cpu::LD_X_Y(&c, h); };
    instructionTable[0x4D] = [this] { gb::cpu::LD_X_Y(&c, l); };
    instructionTable[0x4E] = [this] { gb::cpu::LD_X_DHL(&c); };
    instructionTable[0x4F] = [this] { gb::cpu::LD_X_Y(&c, a); };
    
    instructionTable[0x50] = [this] { gb::cpu::LD_X_Y(&d, b); };
    instructionTable[0x51] = [this] { gb::cpu::LD_X_Y(&d, c); };
    instructionTable[0x52] = [this] { gb::cpu::LD_X_Y(&d, d); };
    instructionTable[0x53] = [this] { gb::cpu::LD_X_Y(&d, e); };
    instructionTable[0x54] = [this] { gb::cpu::LD_X_Y(&d, h); };
    instructionTable[0x55] = [this] { gb::cpu::LD_X_Y(&d, l); };
    instructionTable[0x56] = [this] { gb::cpu::LD_X_DHL(&d); };
    instructionTable[0x57] = [this] { gb::cpu::LD_X_Y(&d, a); };
    instructionTable[0x58] = [this] { gb::cpu::LD_X_Y(&e, b); };
    instructionTable[0x59] = [this] { gb::cpu::LD_X_Y(&e, c); };
    instructionTable[0x5A] = [this] { gb::cpu::LD_X_Y(&e, d); };
    instructionTable[0x5B] = [this] { gb::cpu::LD_X_Y(&e, e); };
    instructionTable[0x5C] = [this] { gb::cpu::LD_X_Y(&e, h); };
    instructionTable[0x5D] = [this] { gb::cpu::LD_X_Y(&e, l); };
    instructionTable[0x5E] = [this] { gb::cpu::LD_X_DHL(&e); };
    instructionTable[0x5F] = [this] { gb::cpu::LD_X_Y(&e, a); };

    instructionTable[0x60] = [this] { gb::cpu::LD_X_Y(&h, b); };
    instructionTable[0x61] = [this] { gb::cpu::LD_X_Y(&h, c); };
    instructionTable[0x62] = [this] { gb::cpu::LD_X_Y(&h, d); };
    instructionTable[0x63] = [this] { gb::cpu::LD_X_Y(&h, e); };
    instructionTable[0x64] = [this] { gb::cpu::LD_X_Y(&h, h); };
    instructionTable[0x65] = [this] { gb::cpu::LD_X_Y(&h, l); };
    instructionTable[0x66] = [this] { gb::cpu::LD_X_DHL(&h); };
    instructionTable[0x67] = [this] { gb::cpu::LD_X_Y(&h, a); };
    instructionTable[0x68] = [this] { gb::cpu::LD_X_Y(&l, b); };
    instructionTable[0x69] = [this] { gb::cpu::LD_X_Y(&l, c); };
    instructionTable[0x6A] = [this] { gb::cpu::LD_X_Y(&l, d); };
    instructionTable[0x6B] = [this] { gb::cpu::LD_X_Y(&l, e); };
    instructionTable[0x6C] = [this] { gb::cpu::LD_X_Y(&l, h); };
    instructionTable[0x6D] = [this] { gb::cpu::LD_X_Y(&l, l); };
    instructionTable[0x6E] = [this] { gb::cpu::LD_X_DHL(&l); };
    instructionTable[0x6F] = [this] { gb::cpu::LD_X_Y(&l, a); };

    instructionTable[0x70] = [this] { gb::cpu::LD_HL_X(&b); };
    instructionTable[0x71] = [this] { gb::cpu::LD_HL_X(&c); };
    instructionTable[0x72] = [this] { gb::cpu::LD_HL_X(&d); };
    instructionTable[0x73] = [this] { gb::cpu::LD_HL_X(&e); };
    instructionTable[0x74] = [this] { gb::cpu::LD_HL_X(&h); };
    instructionTable[0x75] = [this] { gb::cpu::LD_HL_X(&l); };
    instructionTable[0x77] = [this] { gb::cpu::LD_HL_X(&a); };
    instructionTable[0x78] = [this] { gb::cpu::LD_X_Y(&a, b); };
    instructionTable[0x79] = [this] { gb::cpu::LD_X_Y(&a, c); };
    instructionTable[0x7A] = [this] { gb::cpu::LD_X_Y(&a, d); };
    instructionTable[0x7B] = [this] { gb::cpu::LD_X_Y(&a, e); };
    instructionTable[0x7C] = [this] { gb::cpu::LD_X_Y(&a, h); };
    instructionTable[0x7D] = [this] { gb::cpu::LD_X_Y(&a, l); };
    instructionTable[0x7E] = [this] { gb::cpu::LD_X_HL(&a); };
    instructionTable[0x7F] = [this] { gb::cpu::LD_X_Y(&a, a); };
    
    instructionTable[0x80] = [this] { gb::cpu::ADD_X(&b); };
    instructionTable[0x81] = [this] { gb::cpu::ADD_X(&c); };
    instructionTable[0x82] = [this] { gb::cpu::ADD_X(&d); };
    instructionTable[0x83] = [this] { gb::cpu::ADD_X(&e); };
    instructionTable[0x84] = [this] { gb::cpu::ADD_X(&h); };
    instructionTable[0x85] = [this] { gb::cpu::ADD_X(&l); };
    instructionTable[0x86] = [this] { gb::cpu::ADD_HL(); };
    instructionTable[0x87] = [this] { gb::cpu::ADD_X(&a); };
    instructionTable[0x89] = [this] { gb::cpu::ADC_X(&c); };

    instructionTable[0x90] = [this] { gb::cpu::SUB_X(b); };
    instructionTable[0x96] = [this] { gb::cpu::SUB_X(memory->read(GetComboRegister(HL))); cycles += 4; };

    instructionTable[0xA0] = [this] { gb::cpu::AND_X(&b); };
    instructionTable[0xA1] = [this] { gb::cpu::AND_X(&c); };
    instructionTable[0xA7] = [this] { gb::cpu::AND_X(&a); };
    instructionTable[0xA9] = [this] { gb::cpu::XOR_X(&c); };
    instructionTable[0xAA] = [this] { gb::cpu::XOR_X(&d); };
    instructionTable[0xAB] = [this] { gb::cpu::XOR_X(&e); };
    instructionTable[0xAC] = [this] { gb::cpu::XOR_X(&h); };
    instructionTable[0xAD] = [this] { gb::cpu::XOR_X(&l); };
    instructionTable[0xAE] = [this] { gb::cpu::XOR_HL(); };
    instructionTable[0xAF] = [this] { gb::cpu::XOR_X(&a); };

    instructionTable[0xB0] = [this] { gb::cpu::OR_X(&b); };
    instructionTable[0xB1] = [this] { gb::cpu::OR_X(&c); };
    instructionTable[0xB6] = [this] { gb::cpu::OR_HL(); };
    instructionTable[0xB7] = [this] { gb::cpu::OR_X(&a); };
    instructionTable[0xBB] = [this] { gb::cpu::CP_X(e); };
    instructionTable[0xBE] = [this] { gb::cpu::CP_HL(); };

    instructionTable[0xC0] = [this] { gb::cpu::RET_NZ(); };
    instructionTable[0xC1] = [this] { gb::cpu::POP_XX(BC); };
    instructionTable[0xC2] = [this] { gb::cpu::JP_CC_NN(!isFlagSet(FLAG_Z)); };
    instructionTable[0xC3] = [this] { gb::cpu::JP_A16(); };
    instructionTable[0xC4] = [this] { gb::cpu::CALL_NZ_NN(); };
    instructionTable[0xC5] = [this] { gb::cpu::PUSH_XX(BC); };
    instructionTable[0xC6] = [this] { gb::cpu::ADD_N(); };
    instructionTable[0xC8] = [this] { gb::cpu::RET_Z(); };
    instructionTable[0xC9] = [this] { gb::cpu::RET(); };
    instructionTable[0xCA] = [this] { gb::cpu::JP_CC_NN(isFlagSet(FLAG_Z)); };
    instructionTable[0xCD] = [this] { gb::cpu::CALL_A16(); };
    instructionTable[0xCE] = [this] { gb::cpu::ADC_A_NN(memory->read(program_counter++)); };
    instructionTable[0xCF] = [this] { gb::cpu::RST_XX(0x08); };

    instructionTable[0xD0] = [this] { gb::cpu::RET_NC(); };
    instructionTable[0xD1] = [this] { gb::cpu::POP_XX(DE); };
    instructionTable[0xD5] = [this] { gb::cpu::PUSH_XX(DE); };
    instructionTable[0xD6] = [this] { gb::cpu::SUB_X(memory->read(program_counter++)); };
    instructionTable[0xD8] = [this] { gb::cpu::RET_C();};
    instructionTable[0xD9] = [this] { gb::cpu::RET(); IME = 1; };
    instructionTable[0xDE] = [this] { gb::cpu::SBC_D8(); };
    instructionTable[0xDF] = [this] { gb::cpu::RST_XX(0x18); };

    instructionTable[0xE0] = [this] { gb::cpu::LDH_A8_A(); };
    instructionTable[0xE1] = [this] { gb::cpu::POP_XX(HL); };
    instructionTable[0xE2] = [this] { gb::cpu::LD_FFC_A(); };
    instructionTable[0xE5] = [this] { gb::cpu::PUSH_XX(HL); };
    instructionTable[0xE6] = [this] { gb::cpu::AND_N(); };
    instructionTable[0xE9] = [this] { gb::cpu::JP_HL(); };
    instructionTable[0xEA] = [this] { gb::cpu::LD_A16_A(); };
    instructionTable[0xEE] = [this] { gb::cpu::XOR_A_D8(); };
    instructionTable[0xEF] = [this] { gb::cpu::RST_XX(0x28); };

    instructionTable[0xF0] = [this] { gb::cpu::LDH_A_A8(); };
    instructionTable[0xF1] = [this] { gb::cpu::POP_XX(AF); };
    instructionTable[0xF3] = [this] { gb::cpu::DI(); };
    instructionTable[0xF5] = [this] { gb::cpu::PUSH_XX(AF); };
    instructionTable[0xF6] = [this] { gb::cpu::OR_D8(); };
    instructionTable[0xF9] = [this] { gb::cpu::LD_SP_HL(); };
    instructionTable[0xFA] = [this] { gb::cpu::LD_A_NN(); };
    instructionTable[0xFB] = [this] { gb::cpu::EI(); }; 
    instructionTable[0xFE] = [this] { gb::cpu::CP_D8(); }; 
    instructionTable[0xFF] = [this] { gb::cpu::RST_XX(0x38); };

    extendedInstructionTable[0x11] = [this]{ gb::cpu::RL_X(&c); };
    extendedInstructionTable[0x18] = [this]{ gb::cpu::RR_X(&b); };
    extendedInstructionTable[0x19] = [this]{ gb::cpu::RR_X(&c); };
    extendedInstructionTable[0x1A] = [this]{ gb::cpu::RR_X(&d); };
    extendedInstructionTable[0x1B] = [this]{ gb::cpu::RR_X(&e); };
    extendedInstructionTable[0x1C] = [this]{ gb::cpu::RR_X(&h); };
    extendedInstructionTable[0x1D] = [this]{ gb::cpu::RR_X(&l); };
    extendedInstructionTable[0x1F] = [this]{ gb::cpu::RR_X(&a); };

    extendedInstructionTable[0x27] = [this]{ gb::cpu::SLA_X(&a); };

    extendedInstructionTable[0x37] = [this]{ gb::cpu::SWAP_X(&a); };
    extendedInstructionTable[0x38] = [this]{ gb::cpu::SRL_X(&b); };
    extendedInstructionTable[0x3F] = [this]{ gb::cpu::SRL_X(&a); };

    extendedInstructionTable[0x41] = [this]{ gb::cpu::BIT_N_X(BIT_0, &c); };

    extendedInstructionTable[0x5F] = [this]{ gb::cpu::BIT_N_X(BIT_3, &a); };

    extendedInstructionTable[0x6F] = [this]{ gb::cpu::BIT_N_X(BIT_5, &a); };

    extendedInstructionTable[0x77] = [this]{ gb::cpu::BIT_N_X(BIT_6, &a); };
    extendedInstructionTable[0x7C] = [this]{ gb::cpu::BIT_N_X(BIT_7, &h); };
    extendedInstructionTable[0x7E] = [this]{ gb::cpu::BIT_N_HL(BIT_7); };
    extendedInstructionTable[0x7F] = [this]{ gb::cpu::BIT_N_X(BIT_7, &a); };

    extendedInstructionTable[0x86] = [this]{ gb::cpu::RES_N_HL(BIT_0); };
    extendedInstructionTable[0x87] = [this]{ gb::cpu::RES_N_X(&a, BIT_0); };

    extendedInstructionTable[0xBE] = [this]{ gb::cpu::RES_N_HL(BIT_7); };

    extendedInstructionTable[0xFE] = [this]{ gb::cpu::SET_N_HL(BIT_7); };
}