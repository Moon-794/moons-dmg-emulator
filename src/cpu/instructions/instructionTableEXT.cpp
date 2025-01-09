#include "cpu/cpu.h"

void gb::cpu::SetupInstructionTableEXT()
{
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

    extendedInstructionTable[0x50] = [this]{ gb::cpu::BIT_N_X(BIT_2, &b); };
    extendedInstructionTable[0x51] = [this]{ gb::cpu::BIT_N_X(BIT_2, &c); };
    extendedInstructionTable[0x52] = [this]{ gb::cpu::BIT_N_X(BIT_2, &d); };
    extendedInstructionTable[0x53] = [this]{ gb::cpu::BIT_N_X(BIT_2, &e); };
    extendedInstructionTable[0x54] = [this]{ gb::cpu::BIT_N_X(BIT_2, &h); };
    extendedInstructionTable[0x55] = [this]{ gb::cpu::BIT_N_X(BIT_2, &l); };
    extendedInstructionTable[0x57] = [this]{ gb::cpu::BIT_N_X(BIT_2, &a); };
    extendedInstructionTable[0x58] = [this]{ gb::cpu::BIT_N_X(BIT_3, &b); };
    extendedInstructionTable[0x59] = [this]{ gb::cpu::BIT_N_X(BIT_3, &c); };
    extendedInstructionTable[0x5A] = [this]{ gb::cpu::BIT_N_X(BIT_3, &d); };
    extendedInstructionTable[0x5B] = [this]{ gb::cpu::BIT_N_X(BIT_3, &e); };
    extendedInstructionTable[0x5C] = [this]{ gb::cpu::BIT_N_X(BIT_3, &h); };
    extendedInstructionTable[0x5D] = [this]{ gb::cpu::BIT_N_X(BIT_3, &l); };
    extendedInstructionTable[0x5F] = [this]{ gb::cpu::BIT_N_X(BIT_3, &a); };

    extendedInstructionTable[0x60] = [this]{ gb::cpu::BIT_N_X(BIT_4, &b); };
    extendedInstructionTable[0x61] = [this]{ gb::cpu::BIT_N_X(BIT_4, &c); };
    extendedInstructionTable[0x62] = [this]{ gb::cpu::BIT_N_X(BIT_4, &d); };
    extendedInstructionTable[0x63] = [this]{ gb::cpu::BIT_N_X(BIT_4, &e); };
    extendedInstructionTable[0x64] = [this]{ gb::cpu::BIT_N_X(BIT_4, &h); };
    extendedInstructionTable[0x65] = [this]{ gb::cpu::BIT_N_X(BIT_4, &l); };
    extendedInstructionTable[0x67] = [this]{ gb::cpu::BIT_N_X(BIT_4, &a); };
    extendedInstructionTable[0x68] = [this]{ gb::cpu::BIT_N_X(BIT_5, &b); };
    extendedInstructionTable[0x69] = [this]{ gb::cpu::BIT_N_X(BIT_5, &c); };
    extendedInstructionTable[0x6A] = [this]{ gb::cpu::BIT_N_X(BIT_5, &d); };
    extendedInstructionTable[0x6B] = [this]{ gb::cpu::BIT_N_X(BIT_5, &e); };
    extendedInstructionTable[0x6C] = [this]{ gb::cpu::BIT_N_X(BIT_5, &h); };
    extendedInstructionTable[0x6D] = [this]{ gb::cpu::BIT_N_X(BIT_5, &l); };

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