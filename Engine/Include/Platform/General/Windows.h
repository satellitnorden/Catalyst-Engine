//Windows.
#if defined(CATALYST_WINDOWS)
#pragma once

#include <Windows.h>
#include <Xinput.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CopyMemory
#undef CreateEvent
#undef CreateSemaphore
#undef max
#undef min
#undef PlaySound

/*
*	Main function signature.
*/
#define MAIN int32 main() NOEXCEPT
#endif
