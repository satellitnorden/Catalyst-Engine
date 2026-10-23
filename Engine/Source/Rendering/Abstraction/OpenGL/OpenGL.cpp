#if defined(CATALYST_HAS_OPEN_GL)
//Header file.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

namespace OpenGL
{
	/*
	*	Retrieves all OpenGL function pointers.
	*/
	void RetrieveOpenGLFunctionPointers() NOEXCEPT
	{
#define OPEN_GL_FUNCTION(TYPE, NAME) NAME = reinterpret_cast<TYPE>(wglGetProcAddress(#NAME)); ASSERT(NAME != nullptr, "Could not retrieve " #NAME " OpenGL function pointer!");
		OPEN_GL_FUNCTIONS
#undef OPEN_GL_FUNCTION
	}
}

//List of all function pointers.
#define OPEN_GL_FUNCTION(TYPE, NAME) TYPE NAME;
OPEN_GL_FUNCTIONS
#undef OPEN_GL_FUNCTION
#endif