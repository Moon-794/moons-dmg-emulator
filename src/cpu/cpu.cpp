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

    //Initialise default interrupt request flags
    memory->write(0xFF0F, 0xE1);
    //Set buttons off
    memory->write(0xFF00, 0xCF);

    SetupInstructionTable();
    SetupInstructionTableEXT();

    fileWriter.open("output.txt");
}

int gb::cpu::Step()
{
    if(memory->read(0xFF44) == memory->read(0xFF45) && memory->read(0xFF44) & BIT_2 != 0)
    {
        memory->write(0xFF45, memory->read(0xFF44) | BIT_2);
        
        if(memory->read(0xFF41) & BIT_6 == BIT_6)
        {
            uint8_t IF = memory->read(0xFF0F);
            IF |= BIT_1;
            memory->write(0xFF0F, IF);
        }
    }
    else
    {
        memory->write(0xFF45, memory->read(0xFF44) & ~BIT_2);
    }

    //Check for interrupt requests
    ProcessInterrupts();

    if(isHalted)
    {
        cycles += 4;
        return 0;
    }

    uint8_t instruction = memory->read(program_counter++);

    if(instruction == 0xCB)
    {
        //Using the extended instruction set, get next byte for opcode
        usingCB = true;
        instruction = memory->read(program_counter++);
    }
    
    //Check if opcode is valid
    if(!usingCB && (this->instructionTable[instruction] == nullptr) || usingCB && (this->extendedInstructionTable[instruction] == nullptr))
    {
        std::cout << "Unknown Instruction Encountered:\n";

        if(usingCB)
            std::cout << "**CB Table**\n";

        memory->PrintByteAsHex(program_counter - 1);
        std::cout << "Cycles: " << std::dec << cycles << "\n";

        exit(-2);
    }
    else
    {
        //Call the instruction implementation
        auto& table = usingCB ? extendedInstructionTable : instructionTable;
        usingCB = false; // Reset usingCB if true
        table[instruction]();
    }

    if(enable_IME_next_instruction)
    {
        enable_IME_next_instruction = false;
        IME = 1;
    }

    return 0;
}

void gb::cpu::ProcessInterrupts()
{
    //IME = INTERRUPT MASTER ENABLE
    if(IME == 1)
    {
        uint8_t IF = memory->read(0xFF0F);
        uint8_t IE = memory->read(0xFFFF);

        //V-Blank Interrupt
        if((IE & BIT_0) == BIT_0 && (IF & BIT_0) == BIT_0)
        {
            //Clear V-Blank Interrupt request
            IF &= ~0x01;
            memory->write(0xFF0F, IF);

            PushPCToStack(0x40);

            //Disable Interrupts
            DI();

            isHalted = false;
        }

        //STAT Interrupt
        if((IE & BIT_1) == BIT_1 && (IF & BIT_1) == BIT_1)
        {
            //Clear serial Interrupt request
            IF &= ~BIT_1;
            memory->write(0xFF0F, IF);

            PushPCToStack(0x48);

            //Disable Interrupts
            DI();

            isHalted = false;
        }

        //Serial Interrupt
        if((IE & BIT_3) == BIT_3 && (IF & BIT_3) == BIT_3)
        {
            //Clear serial Interrupt request
            IF &= ~0x08;
            memory->write(0xFF0F, IF);

            PushPCToStack(0x58);

            //Disable Interrupts
            DI();

            isHalted = false;
        }

        //TIMA interrupt
    }
}

void gb::cpu::PushPCToStack(uint16_t addr)
{
    //Push program counter to stack
    uint8_t pcMSB = program_counter >> 8;
    uint8_t pcLSB = program_counter & 0x00FF;

    stack_pointer--;
    memory->write(stack_pointer, pcMSB);
    stack_pointer--;
    memory->write(stack_pointer, pcLSB);
    
    program_counter = addr;
}

uint16_t gb::cpu::convert16Bit(uint8_t lsb, uint8_t msb)
 {
    return (msb << 8) | lsb;
 }

void gb::cpu::SetComboRegister(RegisterCombo reg, uint16_t data)
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