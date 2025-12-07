#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CopyMemory
#undef CreateDirectory
#undef CreateFile
#undef CreateFont
#undef CreateEvent
#undef CreateSemaphore
#undef FAR
#undef GetCurrentTime
#undef INFINITE
#undef MemoryBarrier
#undef NEAR
#undef OPAQUE
#undef far
#undef max
#undef min
#undef near
#undef PlaySound
#undef Yield
#undef IN
#undef OUT
#undef ERROR
#undef SendMessage
//Because Windows.h has a function called rectangle, but we have a class called Rectangle...
#undef Rectangle
#define Rectangle _Rectangle
#endif