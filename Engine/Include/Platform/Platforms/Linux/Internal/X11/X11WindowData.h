#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Platforms/Linux/Internal/X11/X11Core.h>

class X11WindowData final
{

public:

    //The window.
    X11::Window _Window;

};