#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

#if defined(CATALYST_PLATFORM_WINDOWS)
    #include <Platform/Platforms/Windows/Core/DynamicLibrary.h>
#endif

#if defined(CATALYST_PLATFORM_LINUX)
    #include <Platform/Platforms/Linux/Core/DynamicLibrary.h>
#endif