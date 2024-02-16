#include <iostream>

#include "EmulatorApp.hpp"

int main(int argc, char* argv[])
{
    EmulatorApp::init(argc, argv);
    auto emulator = EmulatorApp::the();
    emulator->run();
    return 0;
}