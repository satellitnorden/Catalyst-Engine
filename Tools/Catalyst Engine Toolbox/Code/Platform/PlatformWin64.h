#pragma once

//Core.
#include <Core/Core.h>

#if defined(PLATFORM_WIN64)
//Constants.
#define PLATFORM_DIRECTORY_SEPARATOR '\\'

//Windows.
#include <objbase.h>

//STD.
#include <string>

namespace PlatformWin64
{

    /*
    *	Generates a GUID.
    */
    static std::string GenerateGUID()
    {
        GUID guid;
        CoCreateGuid(&guid);

        char buffer[64];
        snprintf
        (
            buffer,
            sizeof(buffer),
            "{%08lX-%04hX-%04hX-%04hX-%012llX}",
            guid.Data1,
            guid.Data2,
            guid.Data3,
            (guid.Data4[0] << 8) | guid.Data4[1],
            ((unsigned long long)guid.Data4[2] << 40) |
            ((unsigned long long)guid.Data4[3] << 32) |
            ((unsigned long long)guid.Data4[4] << 24) |
            ((unsigned long long)guid.Data4[5] << 16) |
            ((unsigned long long)guid.Data4[6] << 8) |
            ((unsigned long long)guid.Data4[7])
        );

        return buffer;
    }

}
#endif