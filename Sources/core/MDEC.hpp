/**
 * @file      MDEC.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX MDEC unit for decoding JPEG-like images
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:51 (created)
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

#ifndef MDEC_HPP
#define MDEC_HPP

#include "Forward.hpp"
#include "Component.hpp"
#include "MDECInstruction.hpp"

#include <queue>
#include <array>
#include <memory>
#include <vector>

namespace PSX
{
    /**
     * @brief PSX MDEC
     */
    class MDEC final : public Component
    {
    public:

        static constexpr const u32 MacroblockSize = 8 * 8;

        MDEC(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            reset();
        }
        
        virtual ~MDEC() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief used by DMAChannelMDECIN to check if input data fifo is ready for writing
         */
        bool is_input_fifo_ready() const;

        /**
         * @brief used by DMAChannelMDECOUT to check if output data fifo is ready for reading
         */
        bool is_output_fifo_ready() const;

    private:

        /**
         * @brief reset the MDEC from a command
         */
        void soft_reset();

        /**
         * @brief directly execute the instruction
         */
        void execute(const MDECInstruction&);

        /**
         * @brief modify the control register 
         */
        void write_control(u32);

        /**
         * @brief execute new command or specify parameter for a command 
         */
        void write_command_or_parameter(u32);

        /**
         * @brief update and obtain the status register 
         */
        u32 read_status();

        /**
         * @brief collect the decoded output 
         */
        u32 read_data_or_response();

        /**
         * @brief consumes input and decodes macroblocks from the input fifo 
         */
        void decode_all_macroblocks_from_input_fifo();

        /**
         * @brief decodes 1 macroblock from input fifo (offset is specified by the m_input_fifo_cursor) 
         */
        std::vector<u32> decode_macroblock();

        /**
         * @brief decode 1 component of a macroblock based on a specific quantization table 
         * @return successfully decoded a block
         */
        bool decode_block_with_quantization_table
        (
            std::array<s16, MacroblockSize>& output_block, 
            const std::array<u8, MacroblockSize>& quantization_table
        );

        /**
         * @brief perform inverse discrete cosine transform to convert from frequencies to actual color 
         */
        void do_idct(std::array<s16, MacroblockSize>& block);

        /**
         * @brief convert a decoded macroblock from yuv colorspace to rgb colorspace 
         */
        std::vector<u32> convert_macroblock_from_ycbcr_to_rgb();

        std::shared_ptr<Bus> m_bus;

        /**
         * @brief 0x1F801824 MDEC Status Register (R)
         */
        union Status
        {
            struct
            {
                u32 parameter_remaining_minus_one: 16;
                u32 current_block:                 3; // 0 - Y1, 1 - Y2, 2 - Y3, 3 - Y4, 4 - Cr, 5 - Cb

                u32: 4;

                u32 data_output_bit15:   1;
                u32 data_output_signed:  1;
                u32 data_output_depth:   2; // 0 - 4bit, 1 - 8bit, 2 - 24bit, 3 - 15bit
                u32 data_out_request:    1;
                u32 data_in_request:     1;
                u32 command_busy:        1;
                u32 data_in_fifo_full:   1;
                u32 data_out_fifo_empty: 1;
            };

            u32 raw;
        };

        static_assert(sizeof(Status) == sizeof(u32));

        /**
         * @brief 0x1F801824 MDEC Control/Reset Register (W)
         */
        union Control
        {
            struct
            {
                u32: 29;

                u32 enable_data_out_request: 1;
                u32 enable_data_in_request:  1;
                u32 reset:                   1;
            };

            u32 raw;
        };

        static_assert(sizeof(Control) == sizeof(u32));

        /**
         * MDEC state 
         */
        Status           m_status;  // 0x1F801824 MDEC Status Register (R)
        Control          m_control; // 0x1F801824 MDEC Control/Reset Register (W)
        MDECInstruction  m_current_instruction;
        u16              m_command_num_arguments;
        u32              m_current_command_argument_index;
        u32              m_quantization_table_selector;
        u32              m_input_fifo_cursor;
        u32              m_output_fifo_cursor;
        u32              m_read24bit_value_cursor;
        std::vector<u16> m_input_fifo;
        std::vector<u32> m_output_fifo;
        std::array<s16, MacroblockSize> m_idct_table;
        std::array<u8,  MacroblockSize> m_chroma_quantization_table;
        std::array<u8,  MacroblockSize> m_luma_quantization_table;
        std::array<s16, MacroblockSize> m_y_block[4]; // luma is 2x bigger than chroma
        std::array<s16, MacroblockSize> m_cb_block;
        std::array<s16, MacroblockSize> m_cr_block;
    };
}

#endif // MDEC_HPP