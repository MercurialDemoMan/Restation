/**
 * @file      CPUInstruction.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the MIPS R3000 PSX CPU Instruction
 *
 * @version   0.1
 *
 * @date      30. 10. 2023, 17:47 (created)
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

#ifndef CPU_INSTRUCTION_HPP
#define CPU_INSTRUCTION_HPP

#include "Types.hpp"
#include "Macros.hpp"

namespace PSX
{
    /**
     * @brief Instruction of a MIPS R3000 processor
     */
    union CPUInstruction
    {
        CPUInstruction() : 
            raw(0)
        {

        }

        CPUInstruction(u32 value) :
            raw(value)
        {
            
        }

        struct
        {
            u32 function: 6;             /// special function operation code
            u32 shift: 5;                /// shift amount
            u32 register_destination: 5; /// destination register index
            u32 register_target: 5;      /// target register index
            u32 register_source: 5;      /// source register index
            u32 opcode: 6;               /// operation code
        };
        
        struct
        {
            u32 target: 26; /// jump target address
            u32: 6;
        };
        
        struct
        {
            u32 immediate: 16; /// immediate unsigned value
            u32: 16;
        };

        struct
        {
            s32 immediate_signed: 16; /// immediate signed value
            u32: 16;
        };
        
        u32 raw; /// raw representation of the instruction
    };

    /*
     * make sure instruction is exactly 32-bits long 
     */
    static_assert(sizeof(CPUInstruction) == sizeof(u32), "CPUInstruction has to be 4 bytes long");

    /*
     * make sure bit fields are correctly aligned
     */
    static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "big endian architecture is unsupported");

    /**
     * list of opcodes and their associated number 
     */
    enum class BaseOpcode : u32
    {
        ExtendedFunction               = 0,
        Branch                         = 1,
        Jump                           = 2,
        JumpAndLink                    = 3,
        BranchIfEqual                  = 4,
        BranchIfNotEqual               = 5,
        BranchIfLessThanZero           = 6,
        BranchIfGreaterThanZero        = 7,
        AddImmediate                   = 8,
        AddImmediateUnsigned           = 9,
        SetOnLessThanImmediate         = 10,
        SetOnLessThanImmediateUnsigned = 11,
        AndImmediate                   = 12,
        OrImmediate                    = 13,
        XorImmediate                   = 14,
        LoadUpperImmediate             = 15,
        Coprocessor0                   = 16,
        Coprocessor1                   = 17,
        Coprocessor2                   = 18,
        Coprocessor3                   = 19,
        /* 20 ... 31 */
        LoadByte             = 32,
        LoadHalfWord         = 33,
        LoadWordLeft         = 34,
        LoadWord             = 35,
        LoadByteUnsigned     = 36,
        LoadHalfWordUnsigned = 37,
        LoadWordRight        = 38,
        /* 39 */
        StoreByte      = 40,
        StoreHalfWord  = 41,
        StoreWordLeft  = 42,
        StoreWord      = 43,
        /* 44 .. 45 */
        StoreWordRight = 46,
        /* 47 */
        LoadWordToCoprocessor0 = 48,
        LoadWordToCoprocessor1 = 49,
        LoadWordToCoprocessor2 = 50,
        LoadWordToCoprocessor3 = 51,
        /* 52 ... 55 */
        StoreWordFromCoprocessor0 = 56,
        StoreWordFromCoprocessor1 = 57,
        StoreWordFromCoprocessor2 = 58,
        StoreWordFromCoprocessor3 = 59
    };

    enum class ExtendedFunctionOpcode : u32
    {
        ShiftWordLeftLogical = 0,
        /* 1 */
        ShiftWordRightLogical        = 2,
        ShiftWordRightArithmetic     = 3,
        ShiftWordLeftLogicalVariable = 4,
        /* 5 */
        ShiftWordRightLogicalVariable    = 6,
        ShiftWordRightArithmeticVariable = 7,
        JumpRegister                     = 8,
        JumpAndLinkRegister              = 9,
        /* 10 ... 11 */
        SystemCall = 12,
        Break      = 13,
        /* 14 .. 15 */
        MoveFromHigh = 16,
        MoveToHigh   = 17,
        MoveFromLow  = 18,
        MoveToLow    = 19,
        /* 20 ... 23 */
        Multiply         = 24,
        MultiplyUnsigned = 25,
        Divide           = 26,
        DivideUnsigned   = 27,
        /* 28 ... 31 */
        Add              = 32,
        AddUnsigned      = 33,
        Subtract         = 34,
        SubtractUnsigned = 35,
        And              = 36,
        Or               = 37,
        Xor              = 38,
        Nor              = 39,
        /* 40 ... 41 */
        SetOnLessThan         = 42,
        SetOnLessThanUnsigned = 43
    };
}

#endif // CPU_INSTRUCTION