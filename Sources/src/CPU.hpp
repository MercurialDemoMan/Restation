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
            
        }

        virtual ~CPU() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

    private:

        /**
         * base instructions
         */
        void UNK(const CPUInstruction&);
        void FUN(const CPUInstruction&);
        void B(const CPUInstruction&);
        void J(const CPUInstruction&);
        void JAL(const CPUInstruction&);
        void BEQ(const CPUInstruction&);
        void BNE(const CPUInstruction&);
        void BLEZ(const CPUInstruction&);
        void BGTZ(const CPUInstruction&);
        void ADDI(const CPUInstruction&);
        void ADDIU(const CPUInstruction&);
        void SLTI(const CPUInstruction&);
        void SLTIU(const CPUInstruction&);
        void ANDI(const CPUInstruction&);
        void ORI(const CPUInstruction&);
        void XORI(const CPUInstruction&);
        void LUI(const CPUInstruction&);
        void COP0(const CPUInstruction&);
        void COP1(const CPUInstruction&);
        void COP2(const CPUInstruction&);
        void COP3(const CPUInstruction&);
        void LB(const CPUInstruction&);
        void LH(const CPUInstruction&);
        void LWL(const CPUInstruction&);
        void LW(const CPUInstruction&);
        void LBU(const CPUInstruction&);
        void LHU(const CPUInstruction&);
        void LWR(const CPUInstruction&);
        void SB(const CPUInstruction&);
        void SH(const CPUInstruction&);
        void SWL(const CPUInstruction&);
        void SW(const CPUInstruction&);
        void SWR(const CPUInstruction&);
        void LWC0(const CPUInstruction&);
        void LWC1(const CPUInstruction&);
        void LWC2(const CPUInstruction&);
        void LWC3(const CPUInstruction&);
        void SWC0(const CPUInstruction&);
        void SWC1(const CPUInstruction&);
        void SWC2(const CPUInstruction&);
        void SWC3(const CPUInstruction&);
        
        /**
         * extended function instructions
         */
        void SLL(const CPUInstruction&);
        void SRL(const CPUInstruction&);
        void SRA(const CPUInstruction&);
        void SLLV(const CPUInstruction&);
        void SRLV(const CPUInstruction&);
        void SRAV(const CPUInstruction&);
        void JR(const CPUInstruction&);
        void JALR(const CPUInstruction&);
        void SYSCALL(const CPUInstruction&);
        void BREAK(const CPUInstruction&);
        void MFHI(const CPUInstruction&);
        void MTHI(const CPUInstruction&);
        void MFLO(const CPUInstruction&);
        void MTLO(const CPUInstruction&);
        void MULT(const CPUInstruction&);
        void MULTU(const CPUInstruction&);
        void DIV(const CPUInstruction&);
        void DIVU(const CPUInstruction&);
        void ADD(const CPUInstruction&);
        void ADDU(const CPUInstruction&);
        void SUB(const CPUInstruction&);
        void SUBU(const CPUInstruction&);
        void AND(const CPUInstruction&);
        void OR(const CPUInstruction&);
        void XOR(const CPUInstruction&);
        void NOR(const CPUInstruction&);
        void SLT(const CPUInstruction&);
        void SLTU(const CPUInstruction&);
    
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

        std::shared_ptr<Bus> m_bus;  /// connection to the bus

    };
}

#endif // CPU_HPP