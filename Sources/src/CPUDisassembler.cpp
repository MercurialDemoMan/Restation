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

#include "CPUDisassembler.hpp"
#include "Macros.hpp"

namespace PSX
{
    static std::string FUN(const CPUInstruction&)
    {
        UNREACHABLE(); return "";
    }

    static std::string UNK(const CPUInstruction&)
    {
        TODO(); return "";
    }
    
    static std::string B(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string J(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string JAL(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string BEQ(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string BNE(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string BLEZ(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string BGTZ(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string ADDI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string ADDIU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string SLTI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string SLTIU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string ANDI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string ORI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string XORI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string LUI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string COP0(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string COP1(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string COP2(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string COP3(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string LB(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LH(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWL(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LW(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LBU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LHU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SB(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SH(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWL(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SW(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWC0(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWC1(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWC2(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string LWC3(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWC0(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWC1(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWC2(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SWC3(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SLL(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SRL(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SRA(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SLLV(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SRLV(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SRAV(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string JR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string JALR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SYSCALL(const CPUInstruction&)
    {
        return "SYSCALL";
    }

    static std::string BREAK(const CPUInstruction&)
    {
        return "BREAK";
    }

    static std::string MFHI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string MTHI(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string MFLO(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string MTLO(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string MULT(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string MULTU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string DIV(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string DIVU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string ADD(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string ADDU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SUB(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SUBU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string AND(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string OR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string XOR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }
    
    static std::string NOR(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SLT(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

    static std::string SLTU(const CPUInstruction& ins)
    {
        TODO(); return "";
    }

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
        if(instruction.opcode != static_cast<u32>(BaseOpcode::ExtendedFunction))
        {
            return base_opcode_to_string[instruction.opcode](instruction);
        }
        else
        {
            return extended_function_to_string[instruction.opcode](instruction);
        }
    }
}