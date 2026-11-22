#if defined(CATALYST_HAS_OPEN_GL)
//Header file.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Systems.
#include <Systems/LogSystem.h>

namespace OpenGL
{
	/*
	*	Retrieves all OpenGL function pointers.
	*/
	void RetrieveOpenGLFunctionPointers() NOEXCEPT
	{
		const int32 version{ gladLoaderLoadGL() };
		LOG_INFORMATION("Load OpenGL function pointers for version %i.%i", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	}
}
#endif