/**
 * @file      CPUDisassembler.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of MIPS R3000 PSX Central processing unit disassembler
 *
 * @version   0.1
 *
 * @date      3. 11. 2023, 13:08 (created)
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

#include "CPUDisassembler.hpp"

#include "Macros.hpp"

#include <fmt/core.h>

namespace PSX
{
    /**
     * @brief Unknown opcode
     */
    static std::string UNK(const CPUInstruction& ins)
    {
        return fmt::format("UNK 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief ExtendedFunction opcode
     */
    static std::string FUN(const CPUInstruction&)
    {
        UNREACHABLE(); return "";
    }
    
    /**
     * @brief Branch opcode
     */
    static std::string B(const CPUInstruction& ins)
    {
        return fmt::format("B {}", 
            ins.immediate_signed * 4
        );
    }
    
    /**
     * @brief Jump opcode
     */
    static std::string J(const CPUInstruction& ins)
    {
        return fmt::format("J 0x{:08x}", 
            ins.target * 4
        );
    }
    
    /**
     * @brief JumpAndLink opcode
     */
    static std::string JAL(const CPUInstruction& ins)
    {
        return fmt::format("JAL 0x{:08x}", 
            ins.target * 4
        );
    }
    
    /**
     * @brief BranchIfEqual opcode
     */
    static std::string BEQ(const CPUInstruction& ins)
    {
        return fmt::format("BEQ [r{}] [r{}] {}", 
            ins.register_source,
            ins.register_target,
            ins.immediate_signed * 4
        );
    }
    
    /**
     * @brief BranchIfNotEqual opcode
     */
    static std::string BNE(const CPUInstruction& ins)
    {
        return fmt::format("BNE [r{}] [r{}] {}",
            ins.register_source,
            ins.register_target,
            ins.immediate_signed * 4
        );
    }
    
    /**
     * @brief BranchIfLessThanZero opcode
     */
    static std::string BLEZ(const CPUInstruction& ins)
    {
        return fmt::format("BLEZ [r{}] 0x{:08x}", 
            ins.register_source,
            ins.immediate_signed * 4
        );
    }
    
    /**
     * @brief BranchIfGreaterThanZero opcode
     */
    static std::string BGTZ(const CPUInstruction& ins)
    {
        return fmt::format("BGTZ [r{}] 0x{:08x}", 
            ins.register_source,
            ins.immediate_signed * 4
        );
    }
    
    /**
     * @brief AddImmediate opcode
     */
    static std::string ADDI(const CPUInstruction& ins)
    {
        return fmt::format("ADDI [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed
        );
    }
    
    /**
     * @brief AddImmediateUnsigned opcode
     */
    static std::string ADDIU(const CPUInstruction& ins)
    {
        return fmt::format("ADDIU [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate
        );
    }
    
    /**
     * @brief SetOnLessThanImmediate opcode
     */
    static std::string SLTI(const CPUInstruction& ins)
    {
        return fmt::format("SLTI [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed
        );
    }
    
    /**
     * @brief SetOnLessThanImmediateUnsigned opcode
     */
    static std::string SLTIU(const CPUInstruction& ins)
    {
        return fmt::format("SLTIU [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate
        );
    }
    
    /**
     * @brief AndImmediate opcode
     */
    static std::string ANDI(const CPUInstruction& ins)
    {
        return fmt::format("ANDI [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate
        );
    }
    
    /**
     * @brief OrImmediate opcode
     */
    static std::string ORI(const CPUInstruction& ins)
    {
        return fmt::format("ORI [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate
        );
    }
    
    /**
     * @brief XorImmediate opcode
     */
    static std::string XORI(const CPUInstruction& ins)
    {
        return fmt::format("XORI [r{}] [r{}] {}", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate
        );
    }
    
    /**
     * @brief LoadUpperImmediate opcode
     */
    static std::string LUI(const CPUInstruction& ins)
    {
        return fmt::format("LUI [r{}] {}", 
            ins.register_target, 
            ins.immediate << 16
        );
    }
    
    /**
     * @brief Coprocessor0 opcode
     */
    static std::string COP0(const CPUInstruction& ins)
    {
        return fmt::format("COP0 [r{}] [r{}] [r{}]",
            ins.register_source,
            ins.register_target,
            ins.register_destination
        );
    }
    
    /**
     * @brief Coprocessor1 opcode
     */
    static std::string COP1(const CPUInstruction& ins)
    {
        return fmt::format("COP1 0x{:08x}", 
            ins.raw
        );
    }
    
    /**
     * @brief Coprocessor2 opcode
     */
    static std::string COP2(const CPUInstruction& ins)
    {
        return fmt::format("COP2 0x{:08x}", 
            ins.raw
        );
    }
    
    /**
     * @brief Coprocessor3 opcode
     */
    static std::string COP3(const CPUInstruction& ins)
    {
        return fmt::format("COP3 0x{:08x}", 
            ins.raw
        );
    }
    
    /**
     * @brief LoadByte opcode
     */
    static std::string LB(const CPUInstruction& ins)
    {
        return fmt::format("LB [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadHalfWord opcode
     */
    static std::string LH(const CPUInstruction& ins)
    {
        return fmt::format("LH [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadWordLeft opcode
     */
    static std::string LWL(const CPUInstruction& ins)
    {
        return fmt::format("LWL [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadWord opcode
     */
    static std::string LW(const CPUInstruction& ins)
    {
        return fmt::format("LW [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadByteUnsigned opcode
     */
    static std::string LBU(const CPUInstruction& ins)
    {
        return fmt::format("LBU [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadHalfWordUnsigned opcode
     */
    static std::string LHU(const CPUInstruction& ins)
    {
        return fmt::format("LHU [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief LoadWordRight opcode
     */
    static std::string LWR(const CPUInstruction& ins)
    {
        return fmt::format("LWR [r{}] [r{}{}{}]", 
            ins.register_target, 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed)
        );
    }

    /**
     * @brief StoreByte opcode
     */
    static std::string SB(const CPUInstruction& ins)
    {
        return fmt::format("SB [r{}{}{}] [r{}]", 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed),
            ins.register_target
        );
    }

    /**
     * @brief StoreHalfWord opcode
     */
    static std::string SH(const CPUInstruction& ins)
    {
        return fmt::format("SH [r{}{}{}] [r{}]", 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed),
            ins.register_target
        );
    }

    /**
     * @brief StoreWordLeft opcode
     */
    static std::string SWL(const CPUInstruction& ins)
    {
        return fmt::format("SWL [r{}{}{}] [r{}]", 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed),
            ins.register_target
        );
    }

    /**
     * @brief StoreWord opcode
     */
    static std::string SW(const CPUInstruction& ins)
    {
        return fmt::format("SW [r{}{}{}] [r{}]", 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed),
            ins.register_target
        );
    }

    /**
     * @brief StoreWordRight opcode
     */
    static std::string SWR(const CPUInstruction& ins)
    {
        return fmt::format("SWR [r{}{}{}] [r{}]", 
            ins.register_source, 
            ins.immediate_signed < 0 ? "-" : "+", 
            std::abs(ins.immediate_signed),
            ins.register_target
        );
    }

    /**
     * @brief LoadWordToCoprocessor0 opcode
     */
    static std::string LWC0(const CPUInstruction& ins)
    {
        return fmt::format("LWC0 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief LoadWordToCoprocessor1 opcode
     */
    static std::string LWC1(const CPUInstruction& ins)
    {
        return fmt::format("LWC1 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief LoadWordToCoprocessor2 opcode
     */
    static std::string LWC2(const CPUInstruction& ins)
    {
        return fmt::format("LWC2 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief LoadWordToCoprocessor3 opcode
     */
    static std::string LWC3(const CPUInstruction& ins)
    {
        return fmt::format("LWC3 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief StoreWordFromCoprocessor0 opcode
     */
    static std::string SWC0(const CPUInstruction& ins)
    {
        return fmt::format("SWC0 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief StoreWordFromCoprocessor1 opcode
     */
    static std::string SWC1(const CPUInstruction& ins)
    {
        return fmt::format("SWC1 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief StoreWordFromCoprocessor2 opcode
     */
    static std::string SWC2(const CPUInstruction& ins)
    {
        return fmt::format("SWC2 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief StoreWordFromCoprocessor3 opcode
     */
    static std::string SWC3(const CPUInstruction& ins)
    {
        return fmt::format("SWC3 0x{:08x}", 
            ins.raw
        );
    }

    /**
     * @brief ShiftWordLeftLogical opcode
     */
    static std::string SLL(const CPUInstruction& ins)
    {
        return fmt::format("SLL [r{}] [r{}] {}", 
            ins.register_destination, 
            ins.register_target, 
            ins.shift
        );
    }

    /**
     * @brief ShiftWordRightLogical opcode
     */
    static std::string SRL(const CPUInstruction& ins)
    {
        return fmt::format("SRL [r{}] [r{}] {}", 
            ins.register_destination, 
            ins.register_target, 
            ins.shift
        );
    }

    /**
     * @brief ShiftWordRightArithmetic opcode
     */
    static std::string SRA(const CPUInstruction& ins)
    {
        return fmt::format("SRA [r{}] [r{}] {}", 
            ins.register_destination, 
            ins.register_target, 
            ins.shift
        );
    }

    /**
     * @brief ShiftWordLeftLogicalVariable opcode
     */
    static std::string SLLV(const CPUInstruction& ins)
    {
        return fmt::format("SLLV [r{}] [r{}] [r{}]", 
            ins.register_destination, 
            ins.register_target, 
            ins.register_source
        );
    }

    /**
     * @brief ShiftWordRightLogicalVariable opcode
     */
    static std::string SRLV(const CPUInstruction& ins)
    {
        return fmt::format("SRLV [r{}] [r{}] [r{}]", 
            ins.register_destination, 
            ins.register_target, 
            ins.register_source
        );
    }

    /**
     * @brief ShiftWordRightArithmeticVariable opcode
     */
    static std::string SRAV(const CPUInstruction& ins)
    {
        return fmt::format("SRAV [r{}] [r{}] [r{}]", 
            ins.register_destination, 
            ins.register_target, 
            ins.register_source
        );
    }

    /**
     * @brief JumpRegister opcode
     */
    static std::string JR(const CPUInstruction& ins)
    {
        return fmt::format("JR [r{}]", 
            ins.register_source
        );
    }

    /**
     * @brief JumpAndLinkRegister opcode
     */
    static std::string JALR(const CPUInstruction& ins)
    {
        return fmt::format("JALR [r{}]", 
            ins.register_source
        );
    }

    /**
     * @brief SystemCall opcode
     */
    static std::string SYSCALL(const CPUInstruction&)
    {
        return "SYSCALL";
    }

    /**
     * @brief Break opcode
     */
    static std::string BREAK(const CPUInstruction&)
    {
        return "BREAK";
    }

    /**
     * @brief MoveFromHigh opcode
     */
    static std::string MFHI(const CPUInstruction& ins)
    {
        return fmt::format("MFHI [r{}]", 
            ins.register_destination
        );
    }

    /**
     * @brief MoveToHigh opcode
     */
    static std::string MTHI(const CPUInstruction& ins)
    {
        return fmt::format("MTHI [r{}]", 
            ins.register_source
        );
    }

    /**
     * @brief MoveFromLow opcode
     */
    static std::string MFLO(const CPUInstruction& ins)
    {
        return fmt::format("MFLO [r{}]", 
            ins.register_destination
        );
    }

    /**
     * @brief MoveToLow opcode
     */
    static std::string MTLO(const CPUInstruction& ins)
    {
        return fmt::format("MTLO [r{}]", 
            ins.register_source
        );
    }

    /**
     * @brief Multiply opcode
     */
    static std::string MULT(const CPUInstruction& ins)
    {
        return fmt::format("MULT [r{}] [r{}]", 
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief MultiplyUnsigned opcode
     */
    static std::string MULTU(const CPUInstruction& ins)
    {
        return fmt::format("MULTU [r{}] [r{}]", 
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief Divide opcode
     */
    static std::string DIV(const CPUInstruction& ins)
    {
        return fmt::format("DIV [r{}] [r{}]", 
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief DivideUnsigned opcode
     */
    static std::string DIVU(const CPUInstruction& ins)
    {
        return fmt::format("DIVU [r{}] [r{}]", 
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief Add opcode
     */
    static std::string ADD(const CPUInstruction& ins)
    {
        return fmt::format("ADD [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief AddUnsigned opcode
     */
    static std::string ADDU(const CPUInstruction& ins)
    {
        return fmt::format("ADDU [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief Subtract opcode
     */
    static std::string SUB(const CPUInstruction& ins)
    {
        return fmt::format("SUB [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief SubtractUnsigned opcode
     */
    static std::string SUBU(const CPUInstruction& ins)
    {
        return fmt::format("SUBU [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief And opcode
     */
    static std::string AND(const CPUInstruction& ins)
    {
        return fmt::format("AND [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief Or opcode
     */
    static std::string OR(const CPUInstruction& ins)
    {
        return fmt::format("OR [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief Xor opcode
     */
    static std::string XOR(const CPUInstruction& ins)
    {
        return fmt::format("XOR [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }
    
    /**
     * @brief Nor opcode
     */
    static std::string NOR(const CPUInstruction& ins)
    {
        return fmt::format("NOR [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief SetOnLessThan opcode
     */
    static std::string SLT(const CPUInstruction& ins)
    {
        return fmt::format("SLT [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief SetOnLessThanUnsigned opcode
     */
    static std::string SLTU(const CPUInstruction& ins)
    {
        return fmt::format("SLTU [r{}] [r{}] [r{}]",
            ins.register_destination,
            ins.register_source,
            ins.register_target
        );
    }

    /**
     * @brief mapping table of a base opcode to a string parser 
     */
    typedef std::string(*OpcodeToString)(const CPUInstruction&);
    static OpcodeToString base_opcode_to_string[64] =
    {
        &FUN,  &B,     &J,    &JAL,   &BEQ,  &BNE, &BLEZ, &BGTZ,
        &ADDI, &ADDIU, &SLTI, &SLTIU, &ANDI, &ORI, &XORI, &LUI,
        &COP0, &COP1,  &COP2, &COP3,  &UNK,  &UNK, &UNK,  &UNK,
        &UNK,  &UNK,   &UNK,  &UNK,   &UNK,  &UNK, &UNK,  &UNK,
        &LB,   &LH,    &LWL,  &LW,    &LBU,  &LHU, &LWR,  &UNK,
        &SB,   &SH,    &SWL,  &SW,    &UNK,  &UNK, &SWR,  &UNK,
        &LWC0, &LWC1,  &LWC2, &LWC3,  &UNK,  &UNK, &UNK,  &UNK,
        &SWC0, &SWC1,  &SWC2, &SWC3,  &UNK,  &UNK, &UNK,  &UNK,
    };

    /**
     * @brief mapping table of a extended function opcode to a string parser 
     */
    static OpcodeToString extended_function_to_string[64] = 
    {
        &SLL,  &UNK,   &SRL,  &SRA,  &SLLV,    &UNK,   &SRLV, &SRAV,
        &JR,   &JALR,  &UNK,  &UNK,  &SYSCALL, &BREAK, &UNK,  &UNK,
        &MFHI, &MTHI,  &MFLO, &MTLO, &UNK,     &UNK,   &UNK,  &UNK,
        &MULT, &MULTU, &DIV,  &DIVU, &UNK,     &UNK,   &UNK,  &UNK,
        &ADD,  &ADDU,  &SUB,  &SUBU, &AND,     &OR,    &XOR,  &NOR,
        &UNK,  &UNK,   &SLT,  &SLTU, &UNK,     &UNK,   &UNK,  &UNK,
        &UNK,  &UNK,   &UNK,  &UNK,  &UNK,     &UNK,   &UNK,  &UNK,
        &UNK,  &UNK,   &UNK,  &UNK,  &UNK,     &UNK,   &UNK,  &UNK
    };

    std::string disassemble(const CPUInstruction& instruction)
    {
        if(instruction.opcode == static_cast<u32>(BaseOpcode::ExtendedFunction))
        {
            return extended_function_to_string[instruction.function](instruction);
        }
        else
        {
            return base_opcode_to_string[instruction.opcode](instruction);
        }
    }
}