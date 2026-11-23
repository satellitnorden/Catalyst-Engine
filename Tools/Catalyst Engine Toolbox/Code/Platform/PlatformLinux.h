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

    /*
    *   Adds the OpenGL libraries.
    */
    static void AddOpenGLLibraries(std::vector<std::string> *library_includes)
    {
        library_includes->emplace_back("GL.a");
        library_includes->emplace_back("EGL.a");
        library_includes->emplace_back("wayland-egl.a");
    }

    /*
    *   Adds the Assimp libraries.
    */
    static void AddAssimpLibraries(const char *directory, std::vector<std::string> *library_includes)
    {
        library_includes->emplace_back("assimp.so");
    }

}
#endif