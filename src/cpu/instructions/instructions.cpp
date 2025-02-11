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

    a < data ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

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

void gb::cpu::ADD_HL()
{
    uint8_t data = memory->read(GetComboRegister(HL));
    uint8_t result = a + data;

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
    else
        ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);

    if (a + data > 0xFF)
        SetFlag(FLAG_C);
    else
        ResetFlag(FLAG_C);

    a = result;

    cycles += 8;
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

    a = result;
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

void gb::cpu::ADD_HL_SP()
{
    uint16_t firstVal = GetComboRegister(HL);
    uint16_t secondVal = stack_pointer;

    uint32_t result = firstVal + secondVal;

    SetComboRegister(HL, result);

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

void gb::cpu::LD_XX_Y(RegisterCombo regCombo, const uint8_t* reg)
{
    memory->write(GetComboRegister(regCombo), *reg);
    cycles += 8;
}

void gb::cpu::CALL_CC_A16(bool condition)
{
    uint8_t addrLSB = memory->read(program_counter++);
    uint8_t addrMSB = memory->read(program_counter++);

    uint16_t addr = convert16Bit(addrLSB, addrMSB);

    if(condition)
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

void gb::cpu::DAA()
{
    uint16_t val = a;

    if(isFlagSet(FLAG_N))
    {
        if(isFlagSet(FLAG_C))
            val -= 0x60;
        
        if(isFlagSet(FLAG_H))
            val -= 0x06;
    }
    else
    {
        if(isFlagSet(FLAG_C) || val > 0x99)
        {
            val += 0x60;
            SetFlag(FLAG_C);
        }

        if(isFlagSet(FLAG_H) || (val & 0x0F) > 0x09)
            val += 0x06;
    }

    (val & 0xFF) == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    a = val;

    ResetFlag(FLAG_H);

    cycles += 4;
}

void gb::cpu::SBC_X(uint8_t* reg)
{
    uint8_t carry = isFlagSet(FLAG_C) ? 1 : 0;

    uint8_t result = a - carry - (*reg);

    result == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    if(a < ((*reg) + carry))
    {
        SetFlag(FLAG_C);
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    if((a & 0x0F) < ((*reg) & 0x0F) + (carry & 0x0F))
    {
        SetFlag(FLAG_H);
    }
    else
    {
        ResetFlag(FLAG_H);
    }

    SetFlag(FLAG_N);

    a = result;

    cycles += 4;
}

void gb::cpu::SBC_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    SBC_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 4;
}

void gb::cpu::HALT()
{
    isHalted = true;
}

void gb::cpu::AND_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    AND_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 4;
}

void gb::cpu::LDH_A_C()
{
    uint16_t addr = convert16Bit(c, 0xFF);
    a = memory->read(addr);

    cycles += 8;
}

void gb::cpu::ADD_SP_E8()
{
    int8_t offset = static_cast<int8_t>(memory->read(program_counter++));
    uint16_t oldSP = stack_pointer;

    stack_pointer += offset;

    if(((oldSP & 0x0F) + (static_cast<uint8_t>(offset) & 0x0F)) > 0x0F)
        SetFlag(FLAG_H);
    else
        ResetFlag(FLAG_H);
    
    if(((oldSP & 0xFF) + (static_cast<uint8_t>(offset) & 0xFF)) > 0xFF)
        SetFlag(FLAG_C);
    else
        ResetFlag(FLAG_C);
    
    ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);

    cycles += 16;
}

void gb::cpu::LD_HL_SPXE8()
{
    int8_t offset = static_cast<int8_t>(memory->read(program_counter++));
    uint16_t oldSP = stack_pointer;

    uint16_t result = stack_pointer + offset;
    SetComboRegister(HL, result);

    if(((oldSP & 0x0F) + (static_cast<uint8_t>(offset) & 0x0F)) > 0x0F)
        SetFlag(FLAG_H);
    else
        ResetFlag(FLAG_H);
    
    if(((oldSP & 0xFF) + (static_cast<uint8_t>(offset) & 0xFF)) > 0xFF)
        SetFlag(FLAG_C);
    else
        ResetFlag(FLAG_C);
    
    ResetFlag(FLAG_Z);
    ResetFlag(FLAG_N);

    cycles += 12;
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
void gb::cpu::BIT_N_X(uint8_t n, const uint8_t* reg)
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
    uint8_t value = ((*reg) << 1);

    ((*reg) & BIT_7) != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    value == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    *reg = value;

    cycles += 8;
}

void gb::cpu::SRA_X(uint8_t* reg)
{
    uint8_t value = ((*reg) >> 1) & 0xFF;
    
    ((*reg) & BIT_7) != 0 ? value |= BIT_7 : value &= ~BIT_7;
    ((*reg) & BIT_0) != 0 ? SetFlag(FLAG_C) : ResetFlag(FLAG_C);
    value == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);

    ResetFlag(FLAG_N);
    ResetFlag(FLAG_H);

    (*reg) = value;

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

 void gb::cpu::SET_N_X(uint8_t bitNo, uint8_t* reg)
 {
    uint8_t val = (*reg);

    val |= bitNo;
    *reg = val;

    cycles += 8;
 }

 void gb::cpu::RLC_X(uint8_t* reg)
 {
    uint8_t value = (*reg);
    uint8_t leftMostBit = value >> 7;

    value = value << 1;
    
    if(leftMostBit == 1)
    {
        SetFlag(FLAG_C);
        value |= 1;
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    value == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    
    ResetFlag(FLAG_H);
    ResetFlag(FLAG_N);

    *reg = value;
    cycles += 8;
 }

 void gb::cpu::RRC_X(uint8_t* reg)
 {
    uint8_t value = (*reg);
    uint8_t rightMostBit = value & 0x01;

    value = value >> 1;
    
    if(rightMostBit == 1)
    {
        SetFlag(FLAG_C);
        value |= BIT_7;
    }
    else
    {
        ResetFlag(FLAG_C);
    }

    value == 0 ? SetFlag(FLAG_Z) : ResetFlag(FLAG_Z);
    
    ResetFlag(FLAG_H);
    ResetFlag(FLAG_N);

    *reg = value;
    cycles += 8;
 }

 void gb::cpu::SWAP_HL()
 {
    uint8_t value = memory->read(GetComboRegister(HL));
    SWAP_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
 }

void gb::cpu::SRL_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    SRL_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::SLA_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    SLA_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::SRA_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    SRA_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::RL_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    RL_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::RR_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    RR_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::RLC_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    RLC_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}

void gb::cpu::RRC_HL()
{
    uint8_t value = memory->read(GetComboRegister(HL));
    RRC_X(&value);
    memory->write(GetComboRegister(HL), value);
    cycles += 8;
}