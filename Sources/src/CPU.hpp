/**
 * @file      CPU.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the MIPS R3000 PSX Central processing unit
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:20 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2023
 *
 * This file is part of TODO: project. TODO: project is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TODO: project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TODO: project. If not, see http://www.gnu.org/licenses/.
 */

#ifndef CPU_HPP
#define CPU_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "CPUInstruction.hpp"
#include "ExceptionController.hpp"

namespace PSX
{
    /**
     * @brief MIPS R3000 PSX Central processing unit
     */
    class CPU final : public Component
    {
    public:

        CPU(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            initialize_coprocessors();
            reset();
        }

        virtual ~CPU() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

        /**
         * @brief checks whether the cpu cache is isolated by the exception status register
         */
        bool is_cache_isolated() const;

        /**
         * @brief exception manager controller getter
         */
        std::shared_ptr<ExceptionController> exception_controller();

    private:

        /**
         * base instructions
         */
        void UNK(const CPUInstruction&);   /// Unknown opcode
        void FUN(const CPUInstruction&);   /// ExtendedFunction opcode
        void B(const CPUInstruction&);     /// Branch opcode
        void J(const CPUInstruction&);     /// Jump opcode
        void JAL(const CPUInstruction&);   /// JumpAndLink opcode
        void BEQ(const CPUInstruction&);   /// BranchIfEqual opcode
        void BNE(const CPUInstruction&);   /// BranchIfNotEqual opcode 
        void BLEZ(const CPUInstruction&);  /// BranchIfLessThanZero opcode
        void BGTZ(const CPUInstruction&);  /// BranchIfGreaterThanZero opcode
        void ADDI(const CPUInstruction&);  /// AddImmediate opcode
        void ADDIU(const CPUInstruction&); /// AddImmediateUnsigned opcode
        void SLTI(const CPUInstruction&);  /// SetOnLessThanImmediate opcode
        void SLTIU(const CPUInstruction&); /// SetOnLessThanImmediateUnsigned opcode
        void ANDI(const CPUInstruction&);  /// AndImmediate opcode
        void ORI(const CPUInstruction&);   /// OrImmediate opcode
        void XORI(const CPUInstruction&);  /// XorImmediate opcode
        void LUI(const CPUInstruction&);   /// LoadUpperImmediate opcode
        void COP0(const CPUInstruction&);  /// Coprocessor0 opcode
        void COP1(const CPUInstruction&);  /// Coprocessor1 opcode
        void COP2(const CPUInstruction&);  /// Coprocessor2 opcode
        void COP3(const CPUInstruction&);  /// Coprocessor3 opcode
        void LB(const CPUInstruction&);    /// LoadByte opcode
        void LH(const CPUInstruction&);    /// LoadHalfWord opcode
        void LWL(const CPUInstruction&);   /// LoadWordLeft opcode
        void LW(const CPUInstruction&);    /// LoadWord opcode
        void LBU(const CPUInstruction&);   /// LoadByteUnsigned opcode
        void LHU(const CPUInstruction&);   /// LoadHalfWordUnsigned opcode
        void LWR(const CPUInstruction&);   /// LoadWordRight opcode
        void SB(const CPUInstruction&);    /// StoreByte opcode
        void SH(const CPUInstruction&);    /// StoreHalfWord opcode
        void SWL(const CPUInstruction&);   /// StoreWordLeft opcode
        void SW(const CPUInstruction&);    /// StoreWord opcode
        void SWR(const CPUInstruction&);   /// StoreWordRight opcode
        void LWC0(const CPUInstruction&);  /// LoadWordToCoprocessor0 opcode
        void LWC1(const CPUInstruction&);  /// LoadWordToCoprocessor1 opcode
        void LWC2(const CPUInstruction&);  /// LoadWordToCoprocessor2 opcode
        void LWC3(const CPUInstruction&);  /// LoadWordToCoprocessor3 opcode
        void SWC0(const CPUInstruction&);  /// StoreWordFromCoprocessor0 opcode
        void SWC1(const CPUInstruction&);  /// StoreWordFromCoprocessor1 opcode
        void SWC2(const CPUInstruction&);  /// StoreWordFromCoprocessor2 opcode
        void SWC3(const CPUInstruction&);  /// StoreWordFromCoprocessor3 opcode
        
        /**
         * extended function instructions
         */
        void SLL(const CPUInstruction&);     /// ShiftWordLeftLogical opcode
        void SRL(const CPUInstruction&);     /// ShiftWordRightLogical opcode
        void SRA(const CPUInstruction&);     /// ShiftWordRightArithmetic opcode
        void SLLV(const CPUInstruction&);    /// ShiftWordLeftLogicalVariable opcode
        void SRLV(const CPUInstruction&);    /// ShiftWordRightLogicalVariable opcode
        void SRAV(const CPUInstruction&);    /// ShiftWordRightArithmeticVariable opcode
        void JR(const CPUInstruction&);      /// JumpRegister opcode
        void JALR(const CPUInstruction&);    /// JumpAndLinkRegister opcode
        void SYSCALL(const CPUInstruction&); /// SystemCall opcode
        void BREAK(const CPUInstruction&);   /// Break opcode
        void MFHI(const CPUInstruction&);    /// MoveFromHigh opcode
        void MTHI(const CPUInstruction&);    /// MoveToHigh opcode
        void MFLO(const CPUInstruction&);    /// MoveFromLow opcode
        void MTLO(const CPUInstruction&);    /// MoveToLow opcode
        void MULT(const CPUInstruction&);    /// Multiply opcode
        void MULTU(const CPUInstruction&);   /// MultiplyUnsigned opcode
        void DIV(const CPUInstruction&);     /// Divide opcode
        void DIVU(const CPUInstruction&);    /// DivideUnsigned opcode
        void ADD(const CPUInstruction&);     /// Add opcode
        void ADDU(const CPUInstruction&);    /// AddUnsigned opcode
        void SUB(const CPUInstruction&);     /// Subtract opcode
        void SUBU(const CPUInstruction&);    /// SubtractUnsigned opcode
        void AND(const CPUInstruction&);     /// And opcode
        void OR(const CPUInstruction&);      /// Or opcode
        void XOR(const CPUInstruction&);     /// Xor opcode
        void NOR(const CPUInstruction&);     /// Nor opcode
        void SLT(const CPUInstruction&);     /// SetOnLessThan opcode
        void SLTU(const CPUInstruction&);    /// SetOnLessThanUnsigned opcode
    
        /**
         * base instruction handler map
         */
        typedef void(CPU::*OpcodeHandler)(const CPUInstruction&);
        OpcodeHandler m_base_handlers[64] =
        {
            &CPU::FUN,  &CPU::B,     &CPU::J,    &CPU::JAL,   &CPU::BEQ,  &CPU::BNE, &CPU::BLEZ, &CPU::BGTZ,
            &CPU::ADDI, &CPU::ADDIU, &CPU::SLTI, &CPU::SLTIU, &CPU::ANDI, &CPU::ORI, &CPU::XORI, &CPU::LUI,
            &CPU::COP0, &CPU::COP1,  &CPU::COP2, &CPU::COP3,  &CPU::UNK,  &CPU::UNK, &CPU::UNK,  &CPU::UNK,
            &CPU::UNK,  &CPU::UNK,   &CPU::UNK,  &CPU::UNK,   &CPU::UNK,  &CPU::UNK, &CPU::UNK,  &CPU::UNK,
            &CPU::LB,   &CPU::LH,    &CPU::LWL,  &CPU::LW,    &CPU::LBU,  &CPU::LHU, &CPU::LWR,  &CPU::UNK,
            &CPU::SB,   &CPU::SH,    &CPU::SWL,  &CPU::SW,    &CPU::UNK,  &CPU::UNK, &CPU::SWR,  &CPU::UNK,
            &CPU::LWC0, &CPU::LWC1,  &CPU::LWC2, &CPU::LWC3,  &CPU::UNK,  &CPU::UNK, &CPU::UNK,  &CPU::UNK,
            &CPU::SWC0, &CPU::SWC1,  &CPU::SWC2, &CPU::SWC3,  &CPU::UNK,  &CPU::UNK, &CPU::UNK,  &CPU::UNK,
        };
        
        /**
         * extended function instruction handler map
         */
        OpcodeHandler m_extended_function_handlers[64] =
        {
            &CPU::SLL,  &CPU::UNK,   &CPU::SRL,  &CPU::SRA,  &CPU::SLLV,    &CPU::UNK,   &CPU::SRLV, &CPU::SRAV,
            &CPU::JR,   &CPU::JALR,  &CPU::UNK,  &CPU::UNK,  &CPU::SYSCALL, &CPU::BREAK, &CPU::UNK,  &CPU::UNK,
            &CPU::MFHI, &CPU::MTHI,  &CPU::MFLO, &CPU::MTLO, &CPU::UNK,     &CPU::UNK,   &CPU::UNK,  &CPU::UNK,
            &CPU::MULT, &CPU::MULTU, &CPU::DIV,  &CPU::DIVU, &CPU::UNK,     &CPU::UNK,   &CPU::UNK,  &CPU::UNK,
            &CPU::ADD,  &CPU::ADDU,  &CPU::SUB,  &CPU::SUBU, &CPU::AND,     &CPU::OR,    &CPU::XOR,  &CPU::NOR,
            &CPU::UNK,  &CPU::UNK,   &CPU::SLT,  &CPU::SLTU, &CPU::UNK,     &CPU::UNK,   &CPU::UNK,  &CPU::UNK,
            &CPU::UNK,  &CPU::UNK,   &CPU::UNK,  &CPU::UNK,  &CPU::UNK,     &CPU::UNK,   &CPU::UNK,  &CPU::UNK,
            &CPU::UNK,  &CPU::UNK,   &CPU::UNK,  &CPU::UNK,  &CPU::UNK,     &CPU::UNK,   &CPU::UNK,  &CPU::UNK
        };

        /**
         * @brief registers names and their purposes 
         */
        enum RegisterName
        {
            Zero = 0,
            AssemblerTemporary = 1,
            Value0 = 2,
            Value1 = 3,
            Value3 = 4,
            Argument0 = 5,
            Argument1 = 6,
            Argument2 = 7,
            Temp0 = 8,
            Temp1 = 9,
            Temp2 = 10,
            Temp3 = 11,
            Temp4 = 12,
            Temp5 = 13,
            Temp6 = 14,
            Temp7 = 15,
            Temp8 = 16,
            Temp9 = 17,
            SubVariable0 = 18,
            SubVariable1 = 19,
            SubVariable2 = 20,
            SubVariable3 = 21,
            SubVariable4 = 22,
            SubVariable5 = 23,
            SubVariable6 = 24,
            SubVariable7 = 25,
            Interrupt0 = 26,
            Interrupt1 = 27,
            GlobalPointer = 28,
            StackPointer = 29,
            FramePointer = 30,
            ReturnAddress = 31
        };

        /**
         * @brief structure for keeping track of load delays when setting a register 
         */
        struct LoadDelaySlot
        {
            u32 register_id;
            u32 value;
        };

        /**
         * @brief structure for keeping instruction cache line
         */
        struct CacheLine
        {
            u32 tag;
            u32 value;
        };

        /**
         * @brief enumeration for indexing load delay slots
         */
        enum LoadDelaySlotIndex
        {
            Current = 0,
            Next    = 1
        };

        /**
         * @brief allocate and initialize all coprocessors
         */
        void initialize_coprocessors();

        /**
         * @brief set program counter to a specific address
         */
        void set_program_counter(u32 address);

        /**
         * @brief set register to a specific value 
         */
        void set_register(u32 register_id, u32 value);

        /**
         * @brief make jump and set status flag
         */
        void do_jump(u32 address);

        /**
         * @brief update the delay slots and move any queued registers to the register field
         */
        void update_load_delay_slots();

        /**
         * @brief queue up value to set a register 
         */
        void load_delay_slot(u32 register_id, u32 value);

        /**
         * @brief fetch instruction from memory
         */
        CPUInstruction fetch_instruction(u32 address);

        /**
         * @brief switch execution state to handling exceptions
         */
        void trigger_exception(Exception, u32 address = 0);

        /**
         * connected devices 
         */
        std::shared_ptr<Bus> m_bus;                                  /// connection to the bus
        std::shared_ptr<ExceptionController> m_exception_controller; /// Coprocessor0

        /**
         * cpu state 
         */
        u32  m_program_counter;      /// pointer to the current instruction
        u32  m_program_counter_next; /// pointer to the next instruction
        bool m_branch_delay_active;  /// are we delaying execution after taking branch?
        bool m_branching;            /// are we currently branching?

        u32  m_exception_program_counter;     /// saving state of program counter for potential exception
        bool m_exception_branch_delay_active; /// saving state of active branch delay for potential exception
        bool m_exception_branching;           /// saving state of branching for potential exception

        u32  m_register_field[32];   /// array of general purpose registers
        u32  m_register_high;        /// high register (used for storing multiplication and division results)
        u32  m_register_low;         /// low register (used for storing multiplication and division results)

        LoadDelaySlot m_load_delay_slots[2]; /// keep track for delays when trying to set register

        static constexpr const u32 LoadDelaySlotEmptyRegister = 0;           /// delay slots that have register id set to 0 will be ignored
        static constexpr const u32 PCResetAddress             = 0xBFC0'0000; /// default reset address for program counter
        static constexpr const u32 JumpFilter                 = 0xF000'0000;
        static constexpr const u32 CacheLineValidMask         = 0x8000'0000;
    };
}

#endif // CPU_HPP