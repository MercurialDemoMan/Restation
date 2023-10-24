#ifndef MACROS_HPP
#define MACROS_HPP

#include <cstdio>
#include <cstdlib>

/**
 * @brief mark variable as unused
 * 
 * shuts down compiler warnings 
 */
#define MARK_UNUSED(x) ((void)(x))

/**
 * @brief abort and print information where the abort occured
 */
#define ABORT_WITH_MESSAGE(message) do \
{ \
    std::fprintf(stderr, "Encountered %s at %s:%u, aborting...\n", message, __FILE__, __LINE__); \
    std::exit(1); \
} while(0)

#define UNREACHABLE() ABORT_WITH_MESSAGE("\e[1;91munreachable code\e[0m")
#define TODO()        ABORT_WITH_MESSAGE("\e[1;95mtodo\e[0m")

#endif // MACROS_HPP