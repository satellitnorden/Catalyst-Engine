#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

//Platform.
#include <Platform/Platforms/Linux/Internal/X11/X11Core.h>

class X11WindowData final
{

public:

    //Constants.
    constexpr static uint64 X11_OPEN_GL_CONTEXT_SIZE{ 8 };

    //The window.
    Window _Window;

    //The OpenGL context - Needs to be stored in an opaque struct because of include dependency shenanigans.
    Any<X11_OPEN_GL_CONTEXT_SIZE> _OpenGLContext;

};