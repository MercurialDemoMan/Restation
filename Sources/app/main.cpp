#include "App.hpp"

int main(int argc, char* argv[])
{
    App::init(argc, argv);
    App::the()->run();
    return 0;
}