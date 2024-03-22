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
#include "GTEInstruction.hpp"
#include "CPUDisassembler.hpp"

#include <fmt/core.h>

namespace PSX
{
    void CPU::execute(u32 num_steps)
    {
        for(u32 _ = 0; _ < num_steps; _++)
        {
            // save cpu state for potential exception
            m_exception_program_counter = m_program_counter;
            m_exception_branch_delay_active = m_branch_delay_active;
            m_exception_branching = m_branching;

            // reset branching status
            m_branch_delay_active = false;
            m_branching = false;

            // handle interrupts
            if(m_exception_controller->interrupt_pending())
            {
                trigger_exception(Exception::Interrupt);
            }

            // CPU hit a breakpoint, but host did not acknowledged it yet
            if(m_meta_did_hit_breakpoint)
            {
                return;
            }

            // handle host breakpoints
            if(!m_meta_breakpoints.empty())
            {
                // find breakpoint and trigger it, if current pc is in breakpoints container 
                auto it = m_meta_breakpoints.find(m_program_counter);
                if(it != m_meta_breakpoints.end())
                {
                    m_meta_did_hit_breakpoint = true;
                    m_meta_breakpoints.erase(it);
                    return;
                }
            }

            // get instruction from memory
            m_current_instruction = fetch_instruction(m_program_counter);

            // track last executed instruction
            m_meta_last_executed_instructions[m_meta_last_executed_instruction_index] = 
            {
                m_meta_cycles, m_program_counter, m_current_instruction
            };
            m_meta_last_executed_instruction_index = (m_meta_last_executed_instruction_index + 1) % LastExecutedInstructionsSize;

            // move program counter to the next instruction
            set_program_counter(m_program_counter_next);

            // execute instruction
            (this->*m_base_handlers[m_current_instruction.opcode])(m_current_instruction);

            // update register load delay slots
            update_load_delay_slots();

            // keep track of clock cycles
            m_meta_cycles++;
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

        // reset exceptions
        m_exception_controller->reset();

        // reset meta state
        m_meta_last_executed_instruction_index = 0;
        for(auto& ins: m_meta_last_executed_instructions)
             ins = { 0, 0, CPUInstruction(0x0000'0001) }; // invalid instruction
        
        m_meta_cycles = 0;

        m_exception_controller->reset();
        m_gte->reset();
        m_meta_breakpoints.clear();
        m_meta_did_hit_breakpoint = false;
    }

    void CPU::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_program_counter);
        save_state->serialize_from(m_program_counter_next);
        save_state->serialize_from(m_branch_delay_active);
        save_state->serialize_from(m_branching);
        save_state->serialize_from(m_register_field);
        save_state->serialize_from(m_register_high);
        save_state->serialize_from(m_register_low);
        save_state->serialize_from(m_load_delay_slots[0]);
        save_state->serialize_from(m_load_delay_slots[1]);
        save_state->serialize_from(m_exception_program_counter);
        save_state->serialize_from(m_current_instruction);
        save_state->serialize_from(m_exception_branch_delay_active);
        save_state->serialize_from(m_exception_branching);
        save_state->serialize_from(m_meta_last_executed_instructions);
        save_state->serialize_from(m_meta_last_executed_instruction_index);
        save_state->serialize_from(m_meta_cycles);
        save_state->serialize_from(m_meta_breakpoints);
        save_state->serialize_from(m_meta_did_hit_breakpoint);
        m_gte->serialize(save_state);
        m_exception_controller->serialize(save_state);
    }

    void CPU::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_program_counter);
        save_state->deserialize_to(m_program_counter_next);
        save_state->deserialize_to(m_branch_delay_active);
        save_state->deserialize_to(m_branching);
        save_state->deserialize_to(m_register_field);
        save_state->deserialize_to(m_register_high);
        save_state->deserialize_to(m_register_low);
        save_state->deserialize_to(m_load_delay_slots[0]);
        save_state->deserialize_to(m_load_delay_slots[1]);
        save_state->deserialize_to(m_exception_program_counter);
        save_state->deserialize_to(m_current_instruction);
        save_state->deserialize_to(m_exception_branch_delay_active);
        save_state->deserialize_to(m_exception_branching);
        save_state->deserialize_to(m_meta_last_executed_instructions);
        save_state->deserialize_to(m_meta_last_executed_instruction_index);
        save_state->deserialize_to(m_meta_cycles);
        save_state->deserialize_to(m_meta_breakpoints);
        save_state->deserialize_to(m_meta_did_hit_breakpoint);
        m_gte->deserialize(save_state);
        m_exception_controller->deserialize(save_state);
    }

    /**
     * @brief allocate and initialize all coprocessors
     */
    void CPU::initialize_coprocessors()
    {
        m_exception_controller = std::make_shared<ExceptionController>();
        m_gte                  = std::make_shared<GTE>();
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
        if(m_load_delay_slots[LoadDelaySlotIndex::Current].register_id == register_id)
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
        return CPUInstruction(m_bus->dispatch_read<u32>(address));
    }

    /**
     * @brief switch execution state to handling exceptions
     */
    void CPU::trigger_exception(Exception exception_kind, u32 address/* = 0*/)
    {
        static constexpr const char* exception_name[] =
        {
            "Interrupt",
            "TLBModification",
            "TLBLoad",
            "TLBStore",
            "BadAddressLoad",
            "BadAddressStore",
            "BusErrorInstruction",
            "BusErrorData",
            "SystemCall",
            "Break",
            "Reserved",
            "COPUnusable",
            "Overflow"
        };

        LOG_DEBUG(6, fmt::format("exception triggered: {}", exception_name[static_cast<u32>(exception_kind)]));
        MARK_UNUSED(exception_name);
        
        if(exception_kind == Exception::BadAddressLoad ||
           exception_kind == Exception::BadAddressStore)
        {
            m_exception_controller->set_bad_address(address);
        }

        // if interrupt occurns on a GTE command
        // we need to skip the command otherwise
        // it will get either executed twice, or
        // the GTE result will be faulty
        if(exception_kind == Exception::Interrupt)
        {
            if(CPUInstruction(m_bus->dispatch_read<u32>(m_exception_program_counter)).opcode == static_cast<u32>(BaseOpcode::Coprocessor2))
            {
                m_exception_program_counter += sizeof(CPUInstruction);
            }
        }

        // set exception cause and update flags
        m_exception_controller->enter_exception(exception_kind);

        // set coprocessor number in cop0 cause register
        // if the sr flag in cop0 status register doesnt
        // allow the use of a certain coprocessor...
        // TODO: is this necesarry, since we throw on
        //       COP1 and COP3 instruction anyway? 
        if(exception_kind == Exception::COPUnusable)
        {
            TODO();
        }

        if(exception_kind == Exception::Interrupt)
        {
            m_exception_controller->set_exception_program_counter(m_program_counter);
        }
        else
        {
            m_exception_controller->set_exception_program_counter(m_exception_program_counter);
        }

        // adjust exception when we have branch delay
        if(m_exception_branch_delay_active)
        {
            m_exception_controller->adjust_for_branch_delay(m_exception_branching, m_program_counter);
        }

        u32 exception_handler_address = m_exception_controller->get_handler_address();

        // debugging exception, let's ignore it for now
        if(exception_kind == Exception::Break)
        {
            TODO();
        }

        set_program_counter(exception_handler_address);
    }
    
    /**
     * @brief checks whether the cpu cache is isolated by the exception status register
     */
    bool CPU::is_cache_isolated() const
    {
        return m_exception_controller->is_cache_isolated();
    }

    /**
     * @brief exception manager controller getter
     */
    std::shared_ptr<ExceptionController> CPU::exception_controller() const
    {
        return m_exception_controller;
    }

    /**
     * @brief get number of clock cycles since execution 
     */
    u64 CPU::clock_cycles() const
    {
        return m_meta_cycles;
    }

    /**
     * @brief assess the state of the cpu and return it in a readable form
     */
    std::string CPU::to_string() const
    {
        std::string result;
        result += "CPU:\n";
        result += "    state:\n";
        result += fmt::format("        pc:  0x{:08x}, npc: 0x{:08x}\n", m_program_counter, m_program_counter_next);
        result += fmt::format("        bda: 0x{:08x}, br:  0x{:08x}\n", m_branch_delay_active, m_branching);
        for(u32 i = 0; i < 32; i++)
        {
            if(i % 4 == 0)
                result += "        ";
            result += fmt::format("r{:02}: 0x{:08x}  ", i, m_register_field[i]);
            if(i != 0 && (i + 1) % 4 == 0)
                result += "\n";
        }
        result += fmt::format("        rl:  0x{:08x}\n", m_register_low);
        result += fmt::format("        rh:  0x{:08x}\n", m_register_high);
        result += fmt::format("        lds: <{:02}, 0x{:08x}>\n             <{:02}, 0x{:08x}>\n",
            m_load_delay_slots[LoadDelaySlotIndex::Current].register_id,
            m_load_delay_slots[LoadDelaySlotIndex::Current].value,
            m_load_delay_slots[LoadDelaySlotIndex::Next].register_id,
            m_load_delay_slots[LoadDelaySlotIndex::Next].value
        );
        result += fmt::format("        epc: 0x{:08x}\n", m_exception_program_counter);
        result += fmt::format("        ein: {}\n", disassemble(m_current_instruction));
        result += fmt::format("        ebd: 0x{:08x}, ebr: 0x{:08x}\n", m_exception_branch_delay_active, m_exception_branching);
        result += fmt::format("        last instructions {}:\n", m_meta_last_executed_instruction_index);
        for(u32 i = 0; i < LastExecutedInstructionsSize; i++)
        {
            auto exec_ins = m_meta_last_executed_instructions[
                (m_meta_last_executed_instruction_index + i) % LastExecutedInstructionsSize
            ];
            result += fmt::format("            {} 0x{:08x}: {}\n", exec_ins.cycles, exec_ins.address, disassemble(exec_ins.ins));
        }
        return result;
    }

    /**
     * @brief modify the state of CPU to launch executable file 
     */
    void CPU::meta_load_executable(const std::shared_ptr<ExecutableFile>& executable_file)
    {
        set_program_counter(executable_file->initial_pc());
        set_register(28, executable_file->initial_gp());
        set_register(29, executable_file->initial_sp());
        set_register(30, executable_file->initial_sp());
    }

    /**
     * @brief append breakpoint to the list of breakpoints
     *        cpu will halt it's execution and hit breakpoint 
     *        can be checked using the `meta_did_hit_breakpoint`
     */
    void CPU::meta_add_breakpoint(u32 address)
    {
        m_meta_breakpoints.insert(address);
    }

    /**
     * @brief check if CPU hit a breakpoint and if yes, return
     *        the breakpoint address 
     */
    std::optional<u32> CPU::meta_did_hit_breakpoint() const
    {
        if(m_meta_did_hit_breakpoint)
        {
            return m_program_counter;
        }
        else
        {
            return {};
        }
    }

    /**
     * @brief signal a CPU that a breakpoint was processed and the CPU
     *        can continue in execution
     */
    void CPU::meta_acknowledge_breakpoint()
    {
        m_meta_did_hit_breakpoint = false;
    }

    /**
     * @brief Unknown opcode
     */    
    void CPU::UNK(const CPUInstruction& ins)
    {
        MARK_UNUSED(ins);
        TODO();
    }

    /**
     * @brief ExtendedFunction opcode 
     */
    void CPU::FUN(const CPUInstruction& ins)
    {
        (this->*m_extended_function_handlers[ins.function])(ins);
    }

    /**
     * @brief Branch opcode 
     */
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

    /**
     * @brief Jump opcode 
     */
    void CPU::J(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        do_jump((m_program_counter_next & JumpFilter) | (ins.target * 4));
    }

    /**
     * @brief JumpAndLink opcode 
     */
    void CPU::JAL(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        set_register(RegisterName::ReturnAddress, m_program_counter_next);
        do_jump((m_program_counter_next & JumpFilter) | (ins.target * 4));
    }

    /**
     * @brief BranchIfEqual opcode 
     */
    void CPU::BEQ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(m_register_field[ins.register_source] == m_register_field[ins.register_target])
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    /**
     * @brief BranchIfNotEqual opcode 
     */
    void CPU::BNE(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(m_register_field[ins.register_source] != m_register_field[ins.register_target])
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    /**
     * @brief BranchIfLessThanZero opcode 
     */
    void CPU::BLEZ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(static_cast<s32>(m_register_field[ins.register_source]) <= 0)
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    /**
     * @brief BranchIfGreaterThanZero opcode 
     */
    void CPU::BGTZ(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        if(static_cast<s32>(m_register_field[ins.register_source]) > 0)
            do_jump(m_program_counter + (ins.immediate_signed * 4));
    }

    /**
     * @brief AddImmediate opcode 
     */
    void CPU::ADDI(const CPUInstruction& ins)
    {
        u32 result = m_register_field[ins.register_source] + ins.immediate_signed;

        if(check_overflow_add<u32>(m_register_field[ins.register_source], ins.immediate_signed, result))
        {
            trigger_exception(Exception::Overflow);
        }

        set_register(ins.register_target, result);
    }

    /**
     * @brief AddImmediateUnsigned opcode 
     */
    void CPU::ADDIU(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] + ins.immediate_signed);
    }

    /**
     * @brief SetOnLessThanImmediate opcode 
     */
    void CPU::SLTI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, static_cast<s32>(m_register_field[ins.register_source]) < ins.immediate_signed);
    }

    /**
     * @brief SetOnLessThanImmediateUnsigned opcode 
     */
    void CPU::SLTIU(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] < ins.immediate);
    }

    /**
     * @brief AndImmediate opcode 
     */
    void CPU::ANDI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] & ins.immediate);
    }

    /**
     * @brief OrImmediate opcode 
     */
    void CPU::ORI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] | ins.immediate);
    }

    /**
     * @brief XorImmediate opcode 
     */
    void CPU::XORI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, m_register_field[ins.register_source] ^ ins.immediate);
    }

    /**
     * @brief LoadUpperImmediate opcode 
     */
    void CPU::LUI(const CPUInstruction& ins)
    {
        set_register(ins.register_target, ins.immediate << 16);
    }

    /**
     * @brief Coprocessor0 opcode 
     */
    void CPU::COP0(const CPUInstruction& ins)
    {
        switch(ins.register_source)
        {
            // read from cop0
            case 0:
            {
                load_delay_slot(
                    ins.register_target, 
                    m_exception_controller->read(ins.register_destination)
                );
                return;
            }

            // write to cop0
            case 4:
            {
                m_exception_controller->write(ins.register_destination, m_register_field[ins.register_target]);
                return;
            }

            // return from exception
            case 16:
            {
                m_exception_controller->return_from_exception();
                return;
            }
        }

        UNREACHABLE();
    }

    /**
     * @brief Coprocessor1 opcode 
     */
    void CPU::COP1(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief Coprocessor2 opcode 
     */
    void CPU::COP2(const CPUInstruction& ins)
    {
        auto gte_instruction = GTEInstruction(ins.raw);

        if(gte_instruction.signature == GTEInstruction::CommandSignature)
        {
            m_gte->execute(gte_instruction); return;
        }

        switch(ins.register_source)
        {
            case 0:
            {
                load_delay_slot(ins.register_target, m_gte->read(ins.register_destination)); return;
            } break;
            case 2:
            {
                load_delay_slot(ins.register_target, m_gte->read(ins.register_destination + 32)); return;
            } break;
            case 4:
            {
                m_gte->write(ins.register_destination, m_register_field[ins.register_target]); return;
            } break;
            case 6:
            {
                m_gte->write(ins.register_destination + 32, m_register_field[ins.register_target]); return;
            } break;
        }

        UNREACHABLE();
    }

    /**
     * @brief Coprocessor3 opcode 
     */
    void CPU::COP3(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief LoadByte opcode 
     */
    void CPU::LB(const CPUInstruction& ins)
    {
        u32 address   = m_register_field[ins.register_source] + ins.immediate_signed;
        s8  read_byte = static_cast<s8>(m_bus->dispatch_read<u8>(address));
        //                                   v - sign extend the byte here
        load_delay_slot(ins.register_target, static_cast<s32>(read_byte));
    }

    /**
     * @brief LoadHalfWord opcode 
     */
    void CPU::LH(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        if (address & 0x0000'0001)
        {
            trigger_exception(Exception::BadAddressLoad, address);
            return;
        }

        s16 read_half_word = static_cast<s16>(m_bus->dispatch_read<u16>(address));
        //                                   v - sign extend the byte here
        load_delay_slot(ins.register_target, static_cast<s32>(read_half_word));
    }

    /**
     * @brief LoadWordLeft opcode 
     */
    void CPU::LWL(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value   = m_bus->dispatch_read<u32>(address & 0xFFFF'FFFC);

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
            case 0: { load_delay_slot(ins.register_target, (register_value & 0x00FF'FFFF) | (value << 24)); break; }
            case 1: { load_delay_slot(ins.register_target, (register_value & 0x0000'FFFF) | (value << 16)); break; }
            case 2: { load_delay_slot(ins.register_target, (register_value & 0x0000'00FF) | (value <<  8)); break; }
            case 3: { load_delay_slot(ins.register_target, (register_value & 0x0000'0000) | (value <<  0)); break; }
        }
    }

    /**
     * @brief LoadWord opcode 
     */
    void CPU::LW(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0003)
        {
            trigger_exception(Exception::BadAddressLoad, address);
            return;
        }

        load_delay_slot(ins.register_target, m_bus->dispatch_read<u32>(address));
    }

    /**
     * @brief LoadByteUnsigned opcode 
     */
    void CPU::LBU(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        load_delay_slot(ins.register_target, m_bus->dispatch_read<u8>(address));
    }

    /**
     * @brief LoadHalfWordUnsigned opcode 
     */
    void CPU::LHU(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0001)
        {
            trigger_exception(Exception::BadAddressLoad, address);
            return;
        }

        load_delay_slot(ins.register_target, m_bus->dispatch_read<u16>(address));
    }

    /**
     * @brief LoadWordRight opcode 
     */
    void CPU::LWR(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value   = m_bus->dispatch_read<u32>(address & 0xFFFF'FFFC);

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
            case 0: { load_delay_slot(ins.register_target, (register_value & 0x0000'0000) | (value >>  0)); break; }
            case 1: { load_delay_slot(ins.register_target, (register_value & 0xFF00'0000) | (value >>  8)); break; }
            case 2: { load_delay_slot(ins.register_target, (register_value & 0xFFFF'0000) | (value >> 16)); break; }
            case 3: { load_delay_slot(ins.register_target, (register_value & 0xFFFF'FF00) | (value >> 24)); break; }
        }
    }

    /**
     * @brief StoreByte opcode 
     */
    void CPU::SB(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;
        m_bus->dispatch_write<u8>(address, m_register_field[ins.register_target]);
    }

    /**
     * @brief StoreHalfWord opcode 
     */
    void CPU::SH(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0001)
        {
            trigger_exception(Exception::BadAddressStore, address);
            return;
        }

        m_bus->dispatch_write<u16>(address, m_register_field[ins.register_target]);
    }

    /**
     * @brief StoreWordLeft opcode 
     */
    void CPU::SWL(const CPUInstruction& ins)
    {
        u32 address        = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value          = m_bus->dispatch_read<u32>(address & 0xFFFF'FFFC);
        u32 register_value = m_register_field[ins.register_target];

        switch(address % 4)
        {
            case 0: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFFFF'FF00) | (register_value >> 24)); break; }
            case 1: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFFFF'0000) | (register_value >> 16)); break; }
            case 2: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0xFF00'0000) | (register_value >>  8)); break; }
            case 3: { m_bus->dispatch_write<u32>(address & 0xFFFFFFFC, (value & 0x0000'0000) | (register_value >>  0)); break; }
        }
    }

    /**
     * @brief StoreWord opcode 
     */
    void CPU::SW(const CPUInstruction& ins)
    {
        u32 address = m_register_field[ins.register_source] + ins.immediate_signed;

        if(address & 0x0000'0003)
        {
            trigger_exception(Exception::BadAddressStore, address);
            return;
        }

        m_bus->dispatch_write<u32>(address, m_register_field[ins.register_target]);
    }

    /**
     * @brief StoreWordRight opcode 
     */
    void CPU::SWR(const CPUInstruction& ins)
    {
        u32 address        = m_register_field[ins.register_source] + ins.immediate_signed;
        u32 value          = m_bus->dispatch_read<u32>(address & 0xFFFF'FFFC);
        u32 register_value = m_register_field[ins.register_target];

        switch(address % 4)
        {
            case 0: { m_bus->dispatch_write<u32>(address & 0xFFFF'FFFC, (value & 0x0000'0000) | (register_value <<  0)); break; }
            case 1: { m_bus->dispatch_write<u32>(address & 0xFFFF'FFFC, (value & 0x0000'00FF) | (register_value <<  8)); break; }
            case 2: { m_bus->dispatch_write<u32>(address & 0xFFFF'FFFC, (value & 0x0000'FFFF) | (register_value << 16)); break; }
            case 3: { m_bus->dispatch_write<u32>(address & 0xFFFF'FFFC, (value & 0x00FF'FFFF) | (register_value << 24)); break; }
        }
    }

    /**
     * @brief LoadWordToCoprocessor0 opcode 
     */
    void CPU::LWC0(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief LoadWordToCoprocessor1 opcode 
     */
    void CPU::LWC1(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief LoadWordToCoprocessor2 opcode 
     */
    void CPU::LWC2(const CPUInstruction& ins)
    {
        m_gte->write
        (
            ins.register_target, 
            m_bus->dispatch_read<u32>(m_register_field[ins.register_source] + ins.immediate_signed)
        );
    }

    /**
     * @brief LoadWordToCoprocessor3 opcode 
     */
    void CPU::LWC3(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief StoreWordFromCoprocessor0 opcode 
     */
    void CPU::SWC0(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief StoreWordFromCoprocessor1 opcode 
     */
    void CPU::SWC1(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief StoreWordFromCoprocessor2 opcode 
     */
    void CPU::SWC2(const CPUInstruction& ins)
    {
        m_bus->dispatch_write<u32>
        (
            m_register_field[ins.register_source] + ins.immediate_signed,
            m_gte->read(ins.register_target)
        );
    }

    /**
     * @brief StoreWordFromCoprocessor3 opcode 
     */
    void CPU::SWC3(const CPUInstruction&)
    {
        trigger_exception(Exception::COPUnusable);
    }

    /**
     * @brief ShiftWordLeftLogical opcode 
     */
    void CPU::SLL(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] << ins.shift);
    }

    /**
     * @brief ShiftWordRightLogical opcode 
     */
    void CPU::SRL(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] >> ins.shift);
    }

    /**
     * @brief ShiftWordRightArithmetic opcode 
     */
    void CPU::SRA(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     static_cast<s32>(m_register_field[ins.register_target]) >> ins.shift);
    }

    /**
     * @brief ShiftWordLeftLogicalVariable opcode 
     */
    void CPU::SLLV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] << (m_register_field[ins.register_source] & 0b0001'1111));
    }

    /**
     * @brief ShiftWordRightLogicalVariable opcode 
     */
    void CPU::SRLV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     m_register_field[ins.register_target] >> (m_register_field[ins.register_source] & 0b0001'1111));
    }

    /**
     * @brief ShiftWordRightArithmeticVariable opcode 
     */
    void CPU::SRAV(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, 
                     static_cast<s32>(m_register_field[ins.register_target]) >> (m_register_field[ins.register_source] & 0b0001'1111));
    }

    /**
     * @brief JumpRegister opcode 
     */
    void CPU::JR(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        u32 address = m_register_field[ins.register_source];

        if(address & 0x0000'0003)
        {
            trigger_exception(Exception::BadAddressLoad, address);
            return;
        }

        do_jump(address);
    }

    /**
     * @brief JumpAndLinkRegister opcode 
     */
    void CPU::JALR(const CPUInstruction& ins)
    {
        m_branch_delay_active = true;
        u32 address = m_register_field[ins.register_source];
        set_register(ins.register_destination, m_program_counter_next);

        if(address & 0x0000'0003)
        {
            trigger_exception(Exception::BadAddressLoad, address);
            return;
        }

        do_jump(address);
    }

    /**
     * @brief SystemCall opcode 
     */
    void CPU::SYSCALL(const CPUInstruction&)
    {
        trigger_exception(Exception::SystemCall);
    }

    /**
     * @brief Break opcode 
     */
    void CPU::BREAK(const CPUInstruction&)
    {
        trigger_exception(Exception::Break);
    }

    /**
     * @brief MoveFromHigh opcode 
     */
    void CPU::MFHI(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_high);
    }

    /**
     * @brief MoveToHigh opcode 
     */
    void CPU::MTHI(const CPUInstruction& ins)
    {
        m_register_high = m_register_field[ins.register_source];
    }

    /**
     * @brief MoveFromLow opcode 
     */
    void CPU::MFLO(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_low);
    }

    /**
     * @brief MoveToLow opcode 
     */
    void CPU::MTLO(const CPUInstruction& ins)
    {
        m_register_low = m_register_field[ins.register_source];
    }

    /**
     * @brief Multiply opcode 
     */
    void CPU::MULT(const CPUInstruction& ins)
    {
        u64 result = static_cast<s64>(static_cast<s32>(m_register_field[ins.register_source])) *
                     static_cast<s64>(static_cast<s32>(m_register_field[ins.register_target]));

        m_register_low  = static_cast<u32>((result >>  0) & 0xFFFF'FFFF);
        m_register_high = static_cast<u32>((result >> 32) & 0xFFFF'FFFF);
    }

    /**
     * @brief MultiplyUnsigned opcode 
     */
    void CPU::MULTU(const CPUInstruction& ins)
    {
        u64 result = static_cast<u64>(m_register_field[ins.register_source]) *
                     static_cast<u64>(m_register_field[ins.register_target]);

        m_register_low  = static_cast<u32>((result >>  0) & 0xFFFF'FFFF);
        m_register_high = static_cast<u32>((result >> 32) & 0xFFFF'FFFF);
    }

    /**
     * @brief Divide opcode 
     */
    void CPU::DIV(const CPUInstruction& ins)
    {
        // denominator is 0
        if(m_register_field[ins.register_target] == 0)
        {
            m_register_low  = static_cast<s32>(m_register_field[ins.register_source]) < 0 ? 1 : 0xFFFF'FFFF;
            m_register_high = m_register_field[ins.register_source];
            return;
        }
        // overflow
        if(m_register_field[ins.register_source] == 0x8000'0000 && m_register_field[ins.register_target] == 0xFFFF'FFFF)
        {
            m_register_low  = 0x8000'0000;
            m_register_high = 0; 
            return;
        }
        // division
        m_register_low  = static_cast<s32>(m_register_field[ins.register_source]) / 
                          static_cast<s32>(m_register_field[ins.register_target]);
        // modulo
        m_register_high = static_cast<s32>(m_register_field[ins.register_source]) % 
                          static_cast<s32>(m_register_field[ins.register_target]);
    }

    /**
     * @brief DivideUnsigned opcode 
     */
    void CPU::DIVU(const CPUInstruction& ins)
    {
        // denominator is 0
        if(m_register_field[ins.register_target] == 0)
        {
            m_register_low  = 0xFFFF'FFFF;
            m_register_high = m_register_field[ins.register_source];
            return;
        }

        // division
        m_register_low  = m_register_field[ins.register_source] / 
                          m_register_field[ins.register_target];
        // modulo
        m_register_high = m_register_field[ins.register_source] % 
                          m_register_field[ins.register_target];                 
    }

    /**
     * @brief Add opcode 
     */
    void CPU::ADD(const CPUInstruction& ins)
    {
        u32 result = m_register_field[ins.register_source] + m_register_field[ins.register_target];

        if(check_overflow_add<u32>(m_register_field[ins.register_source], m_register_field[ins.register_target], result))
        {
            trigger_exception(Exception::Overflow);
        }

        set_register(ins.register_destination, result);
    }

    /**
     * @brief AddUnsigned opcode 
     */
    void CPU::ADDU(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] + 
                                               m_register_field[ins.register_target]);
    }

    /**
     * @brief Subtract opcode 
     */
    void CPU::SUB(const CPUInstruction& ins)
    {
        u32 result = m_register_field[ins.register_source] - m_register_field[ins.register_target];

        if(check_underflow_sub<u32>(m_register_field[ins.register_source], m_register_field[ins.register_target], result))
        {
            trigger_exception(Exception::Overflow);
        }

        set_register(ins.register_destination, result);
    }

    /**
     * @brief SubtractUnsigned opcode 
     */
    void CPU::SUBU(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] - 
                                               m_register_field[ins.register_target]);
    }

    /**
     * @brief And opcode 
     */
    void CPU::AND(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] & 
                                               m_register_field[ins.register_target]);
    }

    /**
     * @brief Or opcode 
     */
    void CPU::OR(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] | 
                                               m_register_field[ins.register_target]);
    }

    /**
     * @brief Xor opcode 
     */
    void CPU::XOR(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] ^ 
                                               m_register_field[ins.register_target]);
    }

    /**
     * @brief Nor opcode 
     */
    void CPU::NOR(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, ~(m_register_field[ins.register_source] | 
                                                 m_register_field[ins.register_target]));
    }

    /**
     * @brief SetOnLessThan opcode 
     */
    void CPU::SLT(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, static_cast<s32>(m_register_field[ins.register_source]) < 
                                               static_cast<s32>(m_register_field[ins.register_target]));
    }

    /**
     * @brief SetOnLessThanUnsigned opcode 
     */
    void CPU::SLTU(const CPUInstruction& ins)
    {
        set_register(ins.register_destination, m_register_field[ins.register_source] < 
                                               m_register_field[ins.register_target]);
    }
}