/**
 * @file      CPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of MIPS R3000 PSX Central processing unit
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:43 (created)
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

#include "CPU.hpp"
#include "Bus.hpp"
#include "Utils.hpp"

namespace PSX
{
    void CPU::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 CPU::read(u32 address)
    {
        MARK_UNUSED(address);
        TODO();
    }

    void CPU::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void CPU::reset()
    {
        TODO();
    }

    /**
     * @brief set program counter to a specific address
     */
    void CPU::set_program_counter(u32 address)
    {
        m_program_counter      = address;
        m_program_counter_next = address + sizeof(CPUInstruction);
    }

    /**
     * @brief set register to a specific value 
     */
    void CPU::set_register(u32 register_id, u32 value)
    {
        if(register_id == 0)
            return;

        m_register_field[register_id] = value;

        if(m_load_delay_slots[0].register_id == register_id)
        {
            TODO();
        }
    }

    /**
     * @brief make jump and set status flags
     */
    void CPU::do_jump(u32 address)
    {
        m_program_counter_next = address;
        m_branching            = true;
    }
    
    void CPU::UNK(const CPUInstruction&)
    {
        UNREACHABLE();
    }

    void CPU::FUN(const CPUInstruction& ins)
    {
        
    }

    void CPU::B(const CPUInstruction&)
    {

    }

    void CPU::J(const CPUInstruction&)
    {

    }

    void CPU::JAL(const CPUInstruction&)
    {

    }

    void CPU::BEQ(const CPUInstruction&)
    {

    }

    void CPU::BNE(const CPUInstruction&)
    {

    }

    void CPU::BLEZ(const CPUInstruction&)
    {

    }

    void CPU::BGTZ(const CPUInstruction&)
    {

    }

    void CPU::ADDI(const CPUInstruction& ins)
    {
        
    }

    void CPU::ADDIU(const CPUInstruction&)
    {

    }

    void CPU::SLTI(const CPUInstruction&)
    {

    }

    void CPU::SLTIU(const CPUInstruction&)
    {

    }

    void CPU::ANDI(const CPUInstruction&)
    {

    }

    void CPU::ORI(const CPUInstruction&)
    {

    }

    void CPU::XORI(const CPUInstruction&)
    {

    }

    void CPU::LUI(const CPUInstruction&)
    {

    }

    void CPU::COP0(const CPUInstruction&)
    {

    }

    void CPU::COP1(const CPUInstruction&)
    {

    }

    void CPU::COP2(const CPUInstruction&)
    {

    }

    void CPU::COP3(const CPUInstruction&)
    {

    }

    void CPU::LB(const CPUInstruction&)
    {

    }

    void CPU::LH(const CPUInstruction&)
    {

    }

    void CPU::LWL(const CPUInstruction&)
    {

    }

    void CPU::LW(const CPUInstruction&)
    {

    }

    void CPU::LBU(const CPUInstruction&)
    {

    }

    void CPU::LHU(const CPUInstruction&)
    {

    }

    void CPU::LWR(const CPUInstruction&)
    {

    }

    void CPU::SB(const CPUInstruction&)
    {

    }

    void CPU::SH(const CPUInstruction&)
    {

    }

    void CPU::SWL(const CPUInstruction&)
    {

    }

    void CPU::SW(const CPUInstruction&)
    {

    }

    void CPU::SWR(const CPUInstruction&)
    {

    }

    void CPU::LWC0(const CPUInstruction&)
    {

    }

    void CPU::LWC1(const CPUInstruction&)
    {

    }

    void CPU::LWC2(const CPUInstruction&)
    {

    }

    void CPU::LWC3(const CPUInstruction&)
    {

    }

    void CPU::SWC0(const CPUInstruction&)
    {

    }

    void CPU::SWC1(const CPUInstruction&)
    {

    }

    void CPU::SWC2(const CPUInstruction&)
    {

    }

    void CPU::SWC3(const CPUInstruction&)
    {

    }

    void CPU::SLL(const CPUInstruction&)
    {

    }

    void CPU::SRL(const CPUInstruction&)
    {

    }

    void CPU::SRA(const CPUInstruction&)
    {

    }

    void CPU::SLLV(const CPUInstruction&)
    {

    }

    void CPU::SRLV(const CPUInstruction&)
    {

    }

    void CPU::SRAV(const CPUInstruction&)
    {

    }

    void CPU::JR(const CPUInstruction&)
    {

    }

    void CPU::JALR(const CPUInstruction&)
    {

    }

    void CPU::SYSCALL(const CPUInstruction&)
    {

    }

    void CPU::BREAK(const CPUInstruction&)
    {

    }

    void CPU::MFHI(const CPUInstruction&)
    {

    }

    void CPU::MTHI(const CPUInstruction&)
    {

    }

    void CPU::MFLO(const CPUInstruction&)
    {

    }

    void CPU::MTLO(const CPUInstruction&)
    {

    }

    void CPU::MULT(const CPUInstruction&)
    {

    }

    void CPU::MULTU(const CPUInstruction&)
    {

    }

    void CPU::DIV(const CPUInstruction&)
    {

    }

    void CPU::DIVU(const CPUInstruction&)
    {

    }

    void CPU::ADD(const CPUInstruction&)
    {

    }

    void CPU::ADDU(const CPUInstruction&)
    {

    }

    void CPU::SUB(const CPUInstruction&)
    {

    }

    void CPU::SUBU(const CPUInstruction&)
    {

    }

    void CPU::AND(const CPUInstruction&)
    {

    }

    void CPU::OR(const CPUInstruction&)
    {

    }

    void CPU::XOR(const CPUInstruction&)
    {

    }

    void CPU::NOR(const CPUInstruction&)
    {

    }

    void CPU::SLT(const CPUInstruction&)
    {

    }

    void CPU::SLTU(const CPUInstruction&)
    {

    }
}