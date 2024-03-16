/**
 * @file      Forward.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Forward declaration of all PSX hardware components
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:22 (created)
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

#ifndef FORWARD_HPP
#define FORWARD_HPP

#include "TimerTypes.hpp"

namespace PSX
{
    class Bus;
    class CPU;
    class GPU;
    class SPU;
    class GTE;
    class MDEC;
    class CDROM;
    template<ClockSource> class Timer;
    class IOPorts;
    class SaveState;
    class SerialPort;
    class DMAChannel;
    class Peripherals;
    class DMAChannelSPU;
    class DMAChannelPIO;
    class DMAChannelOTC;
    class DMAChannelGPU;
    class RamController;
    class MemController;
    class DMAController;
    class ExecutableFile;
    class CacheController;
    class DMAChannelCDROM;
    class PeripheralsInput;
    class DMAChannelMDECIN;
    class DMAChannelMDECOUT;
    class ExceptionController;
    class InterruptController;
}

#endif // FORWARD_HPP