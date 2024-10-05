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
    while(program_counter < 256)
    {
        uint8_t instruction = memory->read(program_counter);
        program_counter++;

        if((this->instructionTable[instruction] == nullptr))
        {
            std::cout << "Unknown Instruction Encountered:\n";
            memory->PrintByteAsHex(program_counter - 1);
            return;
        }
 
        (this->*instructionTable[instruction])();
    } 
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