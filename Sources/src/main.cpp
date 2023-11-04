#include <iostream>

#include "Bus.hpp"
#include "MemoryRegion.hpp"
#include "Types.hpp"

#include <fmt/core.h>

using namespace PSX;

int main(int argc, char* argv[])
{
    auto b = Bus::create();

    b->meta_load_bios(argv[1]);

    b->execute(1);
}
