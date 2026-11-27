#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Platforms/Linux/Internal/Wayland/WaylandCore.h>

class WaylandWindowData final
{

public:

	//The surface.
	wl_surface *RESTRICT _Surface{ nullptr };

	//The XDG surface.
	xdg_surface *RESTRICT _XDGSurface{ nullptr };

	//The XDG top level.
	xdg_toplevel *RESTRICT _XDGTopLevel{ nullptr };

	//The decoration.
	zxdg_toplevel_decoration_v1 *RESTRICT _Decoration{ nullptr };

#if defined(CATALYST_HAS_OPEN_GL)
	//The EGL window.
	wl_egl_window *RESTRICT _EGLWindow{ nullptr };

	//The EGL display.
	EGLDisplay _EGLDisplay;

	//The EGL context.
	EGLContext _EGLContext;

	//The EGL surface.
	EGLSurface _EGLSurface;
#endif

	//Denotes whether or not this window is configured yet.
	bool _Configured{ false };

	//Denotes whether or not the mouse has entered this window.
	bool _MouseEntered{ false };

};