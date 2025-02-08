#ifndef GB_CPU
#define GB_CPU

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <functional>
#include "mmu/mmu.h"

//Zero Flag, set if an operation results in a zero value
#define FLAG_Z 0x80
//Subtraction flag, set if an operation involves subtraction
#define FLAG_N 0x40  
//Half carry flag, set if an operation carries a bit from bit 3 to bit 4
#define FLAG_H 0x20  
//Carry flag, set if an operation carries to bit 7
#define FLAG_C 0x10

#define BIT_7 0x80
#define BIT_6 0x40
#define BIT_5 0x20
#define BIT_4 0x10
#define BIT_3 0x08
#define BIT_2 0x04
#define BIT_1 0x02
#define BIT_0 0x01

#define V_BLANK_FLAG 0x01

namespace gb
{
    enum RegisterCombo
    {
        AF,
        BC,
        DE,
        HL
    };
    
    enum Register
    {
        A,
        B,
        C,
        D,
        E,
        F,
        H,
        L
    };

    class cpu
    {
    public:

        cpu(gb::mmu* memory);
        int Step();

        uint32_t GetCycles() { return cycles; }
        uint16_t program_counter = 0x0000;
        uint16_t stack_pointer = 0x0000;
        
        //CPU Registers
        uint8_t a, b, c, d, e, f, h, l;

        bool debug = false;
        bool isHalted = false;

    private:
        bool enable_IME_next_instruction = false;
        uint8_t IME;

        std::ofstream fileWriter;

        uint32_t cycles = 0;

        typedef void (gb::cpu::*InstructionTable)();
        std::function<void()> instructionTable[256];  
        bool usingCB;
        std::function<void()> extendedInstructionTable[256];

        gb::mmu* memory;

        void ProcessInterrupts();

        void SetFlag(uint8_t flag);
        void ResetFlag(uint8_t flag);
        bool isFlagSet(uint8_t flag);

        void SetupInstructionTable();
        void SetupInstructionTableEXT();

        void LogCPUState();
        void SetComboRegister(RegisterCombo reg, uint16_t data);
        uint16_t GetComboRegister(RegisterCombo reg);

        //Utility
        uint16_t convert16Bit(uint8_t lsb, uint8_t msb);

        //Debugging related
        void PrintRegister(Register reg);
        void PrintComboRegister(RegisterCombo reg);

        // ---------------- Instructions ----------------

        // --- Generics --- 
        void LD_NN_D16(RegisterCombo reg16);
        void LD_X_D8(uint8_t* reg);
        void LD_X_Y(uint8_t* x, uint8_t y);
        void LD_X_YY(uint8_t* x, uint16_t yy);
        void LD_XX_Y(RegisterCombo regCombo, const uint8_t* reg);
        void INC_X(uint8_t* reg);
        void LD_HL_X(uint8_t* reg);
        void LD_X_HL(uint8_t* reg);
        void OR_X(uint8_t* reg);
        void XOR_X(uint8_t* reg);
        void DEC_X(uint8_t* reg);
        void DEC_XX(RegisterCombo reg);
        void DEC_I_XX(RegisterCombo reg);
        void PUSH_XX(RegisterCombo reg);
        void POP_XX(RegisterCombo reg);
        void JR_CC_R8(uint8_t flag);
        void SUB_X(uint8_t value);
        void AND_X(uint8_t* reg);
        void INC_XX_VAL(RegisterCombo reg);
        void RST_XX(uint16_t addr);
        void ADD_X(uint8_t* reg);
        void ADD_XX_YY(RegisterCombo first, RegisterCombo second);
        void LD_X_DHL(uint8_t* reg);
        void ADC_X(uint8_t* reg);
        void JR_CC_E(bool condition);
        void JP_CC_NN(bool condition);
        void CP_X(uint8_t value);
        void SBC_D8();
        void SBC_X(uint8_t* reg);
        void CALL_CC_A16(bool condition);

        // --- Specific implementations ---
        void NO_OP();
        void AND_N();
        void JR_D8();
        void LDH_A_A8();
        void LD_FFC_A();
        void LD_HL_A(); 
        void LD_SP_D16();
        void JR_NZ_D8();
        void JP_A16();
        void LD_HL_D8();
        void LD_HL_D16();
        void LD_HL_DEC_A();
        void LD_HL_INC_A();
        void LD_DE_D16();
        void LDH_A8_A();
        void LDH_A_C();
        void LD_A16_A();
        void LD_A_NN();
        void LD_A_HL_INC();
        void CP_HL();
        void ADD_HL();
        void CALL_A16();
        void RET();
        void RET_NZ();
        void RET_NC();
        void RET_Z();
        void CP_D8();
        void DI();
        void EI();
        void CPL();
        void JP_HL();
        void ADD_N();
        void RLCA();
        void RRCA();
        void RRA();
        void RLA();
        void LD_A_HL_DEC();
        void ADD_HL_SP();
        void ADC_A_NN(uint8_t val);
        void XOR_HL();
        void XOR_A_D8();
        void OR_HL();
        void RET_C();
        void DAA();
        void OR_D8();
        void LD_D16_SP();
        void LD_SP_HL();
        void SCF();
        void CCF();
        void SBC_HL();
        void HALT();
        void AND_HL();
        void ADD_SP_E8();
        void LD_HL_SPXE8();

        // --- CB Generics ---
        void BIT_N_X(uint8_t bitNo, const uint8_t* reg);
        void BIT_N_HL(uint8_t bitNo);
        void RL_X(uint8_t* reg);
        void RL_HL();
        void RLC_X(uint8_t* reg);
        void RLC_HL();
        void RRC_X(uint8_t* reg);
        void RRC_HL();
        void SWAP_X(uint8_t* reg);
        void SWAP_HL();
        void RES_N_X(uint8_t* reg, uint8_t bitNo);
        void RES_N_HL(uint8_t bitNo);
        void SET_N_X(uint8_t bitNo, uint8_t* reg);
        void SET_N_HL(uint8_t bitNo);
        void SLA_X(uint8_t* reg);
        void SLA_HL();
        void SRA_X(uint8_t* reg);
        void SRA_HL();
        void SRL_X(uint8_t* reg);
        void SRL_HL();
        void RR_X(uint8_t* reg);
        void RR_HL();
    };
}

#endif