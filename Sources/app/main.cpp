#include <iostream>

#include "EmulatorApp.hpp"

int main(int argc, char* argv[])
{
    EmulatorApp::init(argc, argv);
    EmulatorApp::the()->run();
    return 0;
}