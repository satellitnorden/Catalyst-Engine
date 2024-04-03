//Core.
#include <Core/Essential/CatalystEssential.h>

#if defined(CATALYST_MSVC)

//MSVC.
#include <intrin.h>

namespace Compiler
{

	/*
	*	Triggers a breakpoint.
	*/
	void Breakpoint() NOEXCEPT
	{
		__nop();
		__debugbreak();
	}

}
#endif