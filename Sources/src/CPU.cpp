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
#include "CPUDisassembler.hpp"

#include <fmt/core.h>

namespace PSX
{
    void CPU::execute(u32 num_steps)
    {
        for(u32 _ = 0; _ < num_steps; _++)
        {
            // get instruction from memory or icache
            auto ins = fetch_instruction(m_program_counter);

            static u32 counter = 0;

            LOG(fmt::format("{}, 0x{:08x} -> {} = 0x{:08x}", counter++, m_program_counter, disassemble(ins), ins.raw));

            // move program counter to the next instruction
            set_program_counter(m_program_counter_next);

            // execute instruction
            (this->*m_base_handlers[ins.opcode])(ins);

            // update register load delay slots
            update_load_delay_slots();
        }
    }

    u32 CPU::read(u32 address)
    {
        MARK_UNUSED(address);
        UNREACHABLE();
    }

    void CPU::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        UNREACHABLE();
    }

    void CPU::reset()
    {
        // set program counter to point to the bios
        set_program_counter(PCResetAddress);

        // reset registers
        for(auto& reg: m_register_field)
            reg = 0;

        m_register_high = 0;
        m_register_low = 0;

        // reset delays
        m_branch_delay_active = false;

        for(auto& delay_slot: m_load_delay_slots)
            delay_slot = { LoadDelaySlotEmptyRegister, 0 };
        
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
        // ignore empty delay slot + 0 register
        if(register_id == LoadDelaySlotEmptyRegister)
            return;

        // set the register
        m_register_field[register_id] = value;

        // if we set already queued register, reset the queued delay slot
        if(m_load_delay_slots[LoadDelaySlotIndex::Current].register_id == register_id)
            m_load_delay_slots[LoadDelaySlotIndex::Current].register_id = LoadDelaySlotEmptyRegister;
    }

    /**
     * @brief make jump and set status flag
     */
    void CPU::do_jump(u32 address)
    {
        m_program_counter_next = address;
        m_branching            = true;
    }

    /**
     * @brief update the delay slots and move any queued registers to the register field
     */
    void CPU::update_load_delay_slots()
    {
        // ignore empty delay slot + 0 register
        if(m_load_delay_slots[LoadDelaySlotIndex::Current].register_id != LoadDelaySlotEmptyRegister)
        {
            u32 current_register_id = m_load_delay_slots[LoadDelaySlotIndex::Current].register_id;
            m_register_field[current_register_id] = m_load_delay_slots[LoadDelaySlotIndex::Current].value;
        }

        // move next delay slot to the current one
        m_load_delay_slots[LoadDelaySlotIndex::Current] = m_load_delay_slots[LoadDelaySlotIndex::Next];
        m_load_delay_slots[LoadDelaySlotIndex::Next].register_id = LoadDelaySlotEmptyRegister;
    }

    /**
     * @brief queue up value to set a register 
     */
    void CPU::load_delay_slot(u32 register_id, u32 value)
    {
        // ignore empty delay slot + 0 register
        if(register_id == LoadDelaySlotEmptyRegister)
            return;

        // if we are trying to set already queued register, reset the queued delay slot
        if(register_id == m_load_delay_slots[LoadDelaySlotIndex::Current].register_id)
            m_load_delay_slots[LoadDelaySlotIndex::Current].register_id = LoadDelaySlotEmptyRegister;

        // queue the register value
        m_load_delay_slots[LoadDelaySlotIndex::Next] = { 
            register_id, 
            value 
        };
    }

    /**
     * @brief fetch instruction from memory
     */
    CPUInstruction CPU::fetch_instruction(u32 address)
    {
        //TODO: icache?
        return CPUInstruction(m_bus->dispatch_read<u32>(address));
    }
    
    void CPU::UNK(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::FUN(const CPUInstruction& ins)
    {
        (this->*m_extended_function_handlers[ins.function])(ins);
    }

    void CPU::B(const CPUInstruction& ins)
    {
        bool greater_and_equal = ins.register_target & 1;
        bool is_link           = (ins.register_target & 0x1E) == 0x10;
        bool condition         = false;

        if(greater_and_equal)
        {
            condition = static_cast<s32>(m_register_field[ins.register_source]) >= 0;
        }
        else
        {
            condition = static_cast<s32>(m_register_field[ins.register_source]) < 0;
        }

        if(is_link)
            set_register(RegisterName::ReturnAddress, m_program_counter_next);

        m_branch_delay_active = true;

        if(condition)
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    void CPU::J(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        do_jump((m_program_counter_next & JumpFilter) | (ins.target * 4));
    }

    void CPU::JAL(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        set_register(RegisterName::ReturnAddress, m_program_counter_next);
        do_jump((m_program_counter_next & JumpFilter) | (ins.target * 4));
    }

    void CPU::BEQ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(m_register_field[ins.register_source] == m_register_field[ins.register_target])
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    void CPU::BNE(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(m_register_field[ins.register_source] != m_register_field[ins.register_target])
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    void CPU::BLEZ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(static_cast<s32>(m_register_field[ins.register_source]) <= 0)
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    void CPU::BGTZ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(static_cast<s32>(m_register_field[ins.register_source]) > 0)
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    void CPU::ADDI(const CPUInstruction& ins)
    {
        u32 result = m_register_field[ins.register_source] + ins.immediate_signed;

        if(check_overflow_add<u32>(m_register_field[ins.register_source], ins.immediate_signed, result))
        {
            // TODO: exception
            TODO();
        }

        set_register(ins.register_target, result);
    }

    void CPU::ADDIU(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] + ins.immediate_signed);
    }

    void CPU::SLTI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, static_cast<s32>(m_register_field[ins.register_source]) < ins.immediate_signed);
    }

    void CPU::SLTIU(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] < ins.immediate);
    }

    void CPU::ANDI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] & ins.immediate);
    }

    void CPU::ORI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] | ins.immediate);
    }

    void CPU::XORI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] ^ ins.immediate);
    }

    void CPU::LUI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, ins.immediate << 16);
    }

    void CPU::COP0(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::COP1(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::COP2(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::COP3(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::LB(const CPUInstruction& ins)
    {
        u32 address   = m_register_field[ins.register_source] + ins.immediate_signed;
        s8  read_byte = static_cast<s8>(m_bus->dispatch_read<u8>(address));
        //                                   v - sign extend the byte here
        load_delay_slot(ins.register_target, static_cast<s32>(read_byte));
    }

    void CPU::LH(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        if (address & 0x0000'0001)
        {
            //TODO: exception
            TODO();
        }

        s16 read_half_word = static_cast<s16>(m_bus->dispatch_read<u16>(address));
        //                                   v - sign extend the byte here
        load_delay_slot(ins.register_target, static_cast<s32>(read_half_word));
    }

    void CPU::LWL(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value   = m_bus->dispatch_read<u32>(address & 0xFFFFFFFC);

        u32 register_value = 0;

        if(m_load_delay_slots[LoadDelaySlotIndex::Current].register_id == ins.register_target)
        {
            register_value = m_load_delay_slots[LoadDelaySlotIndex::Current].value;
        }
        else
        {
            register_value = m_register_field[ins.register_target];
        }

        switch(address % 4)
        {
            case 0: { load_delay_slot(ins.register_target, (register_value & 0x00FFFFFF) | (value << 24)); break; }
            case 1: { load_delay_slot(ins.register_target, (register_value & 0x0000FFFF) | (value << 16)); break; }
            case 2: { load_delay_slot(ins.register_target, (register_value & 0x000000FF) | (value <<  8)); break; }
            case 3: { load_delay_slot(ins.register_target, (register_value & 0x00000000) | (value <<  0)); break; }
        }
    }

    void CPU::LW(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0003)
        {
            // TODO: exception
            TODO();
        }

        load_delay_slot(ins.register_target, m_bus->dispatch_read<u32>(address));
    }

    void CPU::LBU(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        load_delay_slot(ins.register_target, m_bus->dispatch_read<u8>(address));
    }

    void CPU::LHU(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0001)
        {
            // TODO: exception
            TODO();
        }

        load_delay_slot(ins.register_target, m_bus->dispatch_read<u16>(address));
    }

    void CPU::LWR(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value   = m_bus->dispatch_read<u32>(address & 0xFFFFFFFC);

        u32 register_value = 0;

        if(m_load_delay_slots[LoadDelaySlotIndex::Current].register_id == ins.register_target)
        {
            register_value = m_load_delay_slots[LoadDelaySlotIndex::Current].value;
        }
        else
        {
            register_value = m_register_field[ins.register_target];
        }

        switch(address % 4)
        {
            case 0: { load_delay_slot(ins.register_target, (register_value & 0x00000000) | (value >>  0)); break; }
            case 1: { load_delay_slot(ins.register_target, (register_value & 0xFF000000) | (value >>  8)); break; }
            case 2: { load_delay_slot(ins.register_target, (register_value & 0xFFFF0000) | (value >> 16)); break; }
            case 3: { load_delay_slot(ins.register_target, (register_value & 0xFFFFFF00) | (value >> 24)); break; }
        }
    }

    void CPU::SB(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        m_bus->dispatch_write<u8>(address, m_register_field[ins.register_target]);
    }

    void CPU::SH(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0001)
        {
            // TODO: exception
            TODO();
        }

        m_bus->dispatch_write<u16>(address, m_register_field[ins.register_target]);
    }

    void CPU::SWL(const CPUInstruction& ins)
    {
        u32 address        = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value          = m_bus->dispatch_read<u32>(address & 0xFFFFFFFC);
        u32 register_value = m_register_field[ins.register_target];

        switch(address % 4)
        {
            case 0: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFFFFFF00) | (register_value >> 24)); break; }
            case 1: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFFFF0000) | (register_value >> 16)); break; }
            case 2: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFF000000) | (register_value >>  8)); break; }
            case 3: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x00000000) | (register_value >>  0)); break; }
        }
    }

    void CPU::SW(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0003)
        {
            //TODO: exception
            TODO();
        }

        m_bus->dispatch_write<u32>(address, m_register_field[ins.register_target]);
    }

    void CPU::SWR(const CPUInstruction& ins)
    {
        u32 address        = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value          = m_bus->dispatch_read<u32>(address & 0xFFFFFFFC);
        u32 register_value = m_register_field[ins.register_target];

        switch(address % 4)
        {
            case 0: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x00000000) | (register_value <<  0)); break; }
            case 1: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x000000FF) | (register_value <<  8)); break; }
            case 2: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x0000FFFF) | (register_value << 16)); break; }
            case 3: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x00FFFFFF) | (register_value << 24)); break; }
        }
    }

    void CPU::LWC0(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::LWC1(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::LWC2(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::LWC3(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SWC0(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SWC1(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SWC2(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SWC3(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SLL(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_target] << ins.shift);
    }

    void CPU::SRL(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] >> ins.shift);
    }

    void CPU::SRA(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     static_cast<s32>(m_register_field[ins.register_target]) >> ins.shift);
    }

    void CPU::SLLV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] << (m_register_field[ins.register_source] & 0b0001'1111));
    }

    void CPU::SRLV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] >> (m_register_field[ins.register_source] & 0b0001'1111));
    }

    void CPU::SRAV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     static_cast<s32>(m_register_field[ins.register_target]) >> (m_register_field[ins.register_source] & 0b0001'1111));
    }

    void CPU::JR(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::JALR(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SYSCALL(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::BREAK(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MFHI(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MTHI(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MFLO(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MTLO(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MULT(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::MULTU(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::DIV(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::DIVU(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::ADD(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::ADDU(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SUB(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SUBU(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::AND(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::OR(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] | m_register_field[ins.register_target]);
    }

    void CPU::XOR(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::NOR(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SLT(const CPUInstruction&)
    {
        TODO();
    }

    void CPU::SLTU(const CPUInstruction&)
    {
        TODO();
    }
}