//Core.
#include <Core/Essential/CatalystEssential.h>

#if defined(CATALYST_CLANG)
namespace Compiler
{

	/*
	*	Triggers a breakpoint.
	*/
	void Breakpoint() NOEXCEPT
	{
		__builtin_debugtrap();
	}

}
#endif