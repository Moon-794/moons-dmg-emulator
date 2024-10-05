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
            memory->PrintByteAsHex(program_counter);
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

    *firstRegister = (data >> 8) & 0xFF;
    *secondRegister = data & 0xF0;
}

uint16_t gb::cpu::GetComboRegister(RegisterCombo reg)
{
    return 0x0000;
}