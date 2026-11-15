#pragma once

//Core.
#include <Core/Core.h>

#if defined(PLATFORM_LINUX)
namespace Platform
{

    /*
    *   Adds the Vulkan 'shaderc' libraries.
    */
    static void AddVulkanShaderCLibraries(const char *directory, std::vector<std::string> *library_includes)
    {
        library_includes->emplace_back("shaderc_combined.a");
    }

}
#endif