#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Wayland includes.
#include <Wayland/xdg-shell-client-protocol.h>
#include <Wayland/xdg-decoration-unstable-v1-client-protocol.h>
#if defined(CATALYST_HAS_OPEN_GL)
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
	#include <wayland-egl.h>
#endif