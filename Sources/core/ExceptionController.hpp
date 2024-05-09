/**
 * @file      ExceptionController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX CPU Exception Controller
 *
 * @version   0.1
 *
 * @date      5. 11. 2023, 14:01 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2023
 *
 * This file is part of Restation. Restation is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restation is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Restation. If not, see http://www.gnu.org/licenses/.
 */

#ifndef EXCEPTIONCONTROLLER_HPP
#define EXCEPTIONCONTROLLER_HPP

#include "Forward.hpp"
#include "Component.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief enumeration of all exception causes 
     */
    enum class Exception
    {
        Interrupt           = 0,
        TLBModification     = 1, // not present in psx
        TLBLoad             = 2, // not present in psx
        TLBStore            = 3, // not present in psx
        BadAddressLoad      = 4,
        BadAddressStore     = 5,
        BusErrorInstruction = 6,
        BusErrorData        = 7,
        SystemCall          = 8,
        Break               = 9,
        Reserved            = 10,
        COPUnusable         = 11,
        Overflow            = 12
    };

    /**
     * @brief PSX CPU Exception Controller
     */
    class ExceptionController final : public Component
    {
    public:

        ExceptionController()
        {
            reset();
        }
        
        virtual ~ExceptionController() override = default;

        virtual void execute(u32) override {};
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief update history of exceptions
         */
        void return_from_exception();

        /**
         * @brief set bad address on missaligned load or store 
         */
        void set_bad_address(u32 address);

        /**
         * @brief update flags and history of exception
         */
        void enter_exception(Exception);

        /**
         * @brief set where the exception happened
         */
        void set_exception_program_counter(u32 program_counter);

        /**
         * @brief return address of the exception handler routine 
         */
        u32  get_handler_address() const;

        /**
         * @brief check whether the interrupt controller sent information about interrupt
         */
        bool interrupt_pending() const;

        /**
         * @brief check whether the cache is isolated 
         */
        bool is_cache_isolated() const;

        /**
         * @brief interrupt controller wants to interrupt cpu 
         */
        void set_interrupt_pending(u32 value);

        /**
         * @brief set the coprocessor number responsible for exception
         */
        void set_coprocessor_number(u32 cop_number);

        /**
         * @brief if cpu is in branch delay adjust the flags 
         */
        void adjust_for_branch_delay(bool branching, u32 program_counter);

    private:

        /**
         * @brief bit-fields accessors for cop0r7 break point control
         */
        union DCIC
        {
            struct
            {
                u32 any_break_hit: 1;
                u32 bpc_code_break_hit: 1;
                u32 bda_data_break_hit: 1;
                u32 bda_data_read_break_hit: 1;
                u32 bda_data_write_break_hit: 1;
                u32 any_jump_break_hit: 1;

                u32: 6;

                u32 jump_redirections: 2;
                u32 unknown: 2;

                u32: 7;

                u32 super_master_enable_1: 1;
                u32 execution_breakpoint_enable: 1;
                u32 data_access_breakpoint_enable: 1;
                u32 break_on_data_read: 1;
                u32 break_on_data_write: 1;
                u32 break_on_any_jump: 1;
                u32 master_enable_for_break_on_any_jump: 1;
                u32 master_enable_for_breaks: 1;
                u32 super_master_enable_2: 1;
            };

            u32 raw;
        };

        /**
         * @brief bit-fields accessors for cop0r12 system status register 
         */
        union SR
        {
            struct
            {
                u32 current_interrupt_enable: 1;
                u32 current_execution_mode: 1;
                u32 previous_interrupt_disable: 1;
                u32 previous_execution_mode: 1;
                u32 old_interrupt_disable: 1;
                u32 old_execution_mode: 1;

                u32: 2;

                u32 interrupt_mask: 8;
                u32 isolate_cache: 1;
                u32 swapped_cache: 1;
                u32 write_zero_as_parity_bits: 1;
                u32 d_cache_contained_data: 1;
                u32 cache_parity_error: 1;
                u32 tlb_shutdown: 1;
                u32 boot_exception_vectors: 1;
                
                u32: 2;

                u32 reverse_endianness: 1; /// doesn't exist in psx

                u32: 2;

                u32 cop0_enable: 1;
                u32 cop1_enable: 1;
                u32 cop2_enable: 1;
                u32 cop3_enable: 1;
            };

            u32 raw;
        };

        /**
         * @brief bit-fields accessors for cop0r13 cause register 
         */
        union Cause
        {
            struct
            {
                u32: 2;

                u32 exception: 5; /// defined in the `Exception` enum

                u32: 1;

                u32 interrupt_pending: 8;

                u32: 12;

                u32 coprocessor_number: 2;
                u32 branching: 1;           /// TODO: is this right? conflicting information about this bit
                u32 branch_delay: 1;
            };
            

            u32 raw;
        };


        u32   m_bpc;       /// breakpoint on execute
        u32   m_bda;       /// breakpoint on data access
        u32   m_jumpdest;  /// randomly memorized jump address
        DCIC  m_dcic;      /// breakpoint control
        u32   m_bad_vaddr; /// breakpoint on bad virtual address
        u32   m_bdam;      /// data access breakpoint mask
        u32   m_bpcm;      /// execute breakpoint mask
        SR    m_sr;        /// system status register
        Cause m_cause;     /// describes the mostly recently recognised exception
        u32   m_epc;       /// return address from trap
        u32   m_prid;      /// processor id
    };
}

#endif // EXCEPTIONCONTROLLER_HPP