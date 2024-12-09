#include "cpu/cpu.h"

#include <iostream>
#include <iomanip>

gb::cpu::cpu(gb::mmu* memory)
{
    this->memory = memory;
    usingCB = false;

    a = 0x00;
    b = 0x00;
    c = 0x00;
    d = 0x00;
    e = 0x00;
    f = 0x00;
    h = 0x00;
    l = 0x00;

    SetupInstructionTables();
}

int gb::cpu::Step()
{
    uint8_t instruction = memory->read(program_counter);
    program_counter++;

    if(instruction == 0xCB)
    {
        //Using the extended instruction set, get next byte for opcode
        usingCB = true;
        instruction = memory->read(program_counter);
        program_counter++;
    }
    
    if(usingCB == false && (this->instructionTable[instruction] == nullptr) || usingCB == true && (this->extendedInstructionTable[instruction] == nullptr))
    {
        std::cout << "Unknown Instruction Encountered:\n";

        if(!usingCB)
        {
            memory->PrintByteAsHex(program_counter - 1);
        }
        else
        {
            std::cout << "**CB Table**\n";
            memory->PrintByteAsHex(program_counter - 1);
        }

        std::cout << "Cycles: " << std::dec << cycles << "\n";
        

        return -1;
    }
    else
    {
        //Call the instruction implementation
        if(usingCB)
        {
            usingCB = false;
            (extendedInstructionTable[instruction])();
        }
        else
        {
            (instructionTable[instruction])();
        }
    }

    return 0;
}

uint16_t gb::cpu::convert16Bit(uint8_t lsb, uint8_t msb)
 {
    return (msb << 8) | lsb;
 }

void::gb::cpu::SetComboRegister(RegisterCombo reg, uint16_t data)
{
    uint8_t* firstRegister;
    uint8_t* secondRegister;

    switch (reg)
    {
    case gb::AF:
        firstRegister = &a;
        secondRegister = &f;
        break;

    case gb::BC:
        firstRegister = &b;
        secondRegister = &c;
        break;

    case gb::DE:
        firstRegister = &d;
        secondRegister = &e;
        break;

    case gb::HL:
        firstRegister = &h;
        secondRegister = &l;
        break;
    }

    if(reg != AF)
    {
        *firstRegister = (data >> 8) & 0xFF;
        *secondRegister = data & 0xFF;
    }
    else
    {
        *firstRegister = (data >> 8) & 0xFF;
        *secondRegister = data & 0xF0;
    }
}

uint16_t gb::cpu::GetComboRegister(RegisterCombo reg)
{
    uint8_t* firstRegister;
    uint8_t* secondRegister;

    switch (reg)
    {
    case gb::AF:
        firstRegister = &a;
        secondRegister = &f;
        break;

    case gb::BC:
        firstRegister = &b;
        secondRegister = &c;
        break;

    case gb::DE:
        firstRegister = &d;
        secondRegister = &e;
        break;

    case gb::HL:
        firstRegister = &h;
        secondRegister = &l;
        break;
    }

    uint16_t value = 0x0000;
    value = (*firstRegister << 8) | *secondRegister; 

    return value;
}

void gb::cpu::PrintComboRegister(RegisterCombo reg)
{
    uint8_t* firstRegister;
    uint8_t* secondRegister;

    const char* title;

    switch (reg)
    {
    case gb::AF:
        title = "AF";
        firstRegister = &a;
        secondRegister = &f;
        break;

    case gb::BC:
        title = "BC";
        firstRegister = &b;
        secondRegister = &c;
        break;

    case gb::DE:
        title = "DE";
        firstRegister = &d;
        secondRegister = &e;
        break;

    case gb::HL:
        title = "HL";
        firstRegister = &h;
        secondRegister = &l;
        break;
    }

    std::cout << title << ": " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)*firstRegister << (int)*secondRegister << "\n";
}

void gb::cpu::PrintRegister(Register reg)
{
    uint8_t* pReg;

    switch (reg)
    {
    case gb::A:
        pReg = &a;
        break;
    
    case gb::B:
        pReg = &b;
        break;

    case gb::C:
        pReg = &c;
        break;

    case gb::D:
        pReg = &d;
        break;

    case gb::E:
        pReg = &e;
        break;

    case gb::F:
        pReg = &f;
        break;

    case gb::H:
        pReg = &h;
        break;

    case gb::L:
        pReg = &l;
        break;
    }

    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)*pReg << "\n";
}

void gb::cpu::SetFlag(uint8_t flag)
{
    f = f | flag;
}

void gb::cpu::ResetFlag(uint8_t flag)
{
    f = f & ~flag;
}

bool gb::cpu::isFlagSet(uint8_t flag)
{
    return (f & flag);
}