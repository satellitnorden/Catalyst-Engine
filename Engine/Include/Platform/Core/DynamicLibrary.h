#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

#if defined(CATALYST_PLATFORM_WINDOWS)
    #include <Platform/Windows/DynamicLibrary.h>
#endif

#if defined(CATALYST_PLATFORM_LINUX)
    #include <Platform/Linux/DynamicLibrary.h>
#endif