//Core.
#include <Core/Essential/CatalystEssential.h>

#if defined(CATALYST_GCC)
namespace Compiler
{

    /*
    *	Triggers a breakpoint.
    */
    void Breakpoint() NOEXCEPT
    {
        //TODO: Figure out a better way to do this. (:
        int *x = nullptr;
        *x = 42;
    }

}
#endif