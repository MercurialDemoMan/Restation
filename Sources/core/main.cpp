#include <iostream>

#include "Bus.hpp"
#include "MemoryRegion.hpp"
#include "Types.hpp"

#include <fmt/core.h>
extern "C"
{
    #include <unistd.h>
    #include <signal.h>
    #include <stdlib.h>
}

std::shared_ptr<PSX::Bus> b;

void sigint_handler(int)
{
    exit(1);
}

using namespace PSX;

int main(int argc, char* argv[])
{
    MARK_UNUSED(argc);
    MARK_UNUSED(argv);

    b = Bus::create();

    struct sigaction sig_action;
    sig_action.sa_handler = sigint_handler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sigaction(SIGINT, &sig_action, NULL);

    b->meta_load_bios(argv[1]);
    while(true)
    {
        b->execute(301);
    }
}
