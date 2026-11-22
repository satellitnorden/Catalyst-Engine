#if defined(CATALYST_HAS_OPEN_GL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <glad/gl.h>

//Macros.
#define OPEN_GL_CALL(X) X; OpenGL::CheckError();

namespace OpenGL
{

	//Type aliases.
	using Identifier = uint32;

	/*
	*	Checks the last OpenGL error.
	*/
	FORCE_INLINE static void CheckError()
	{
		GLenum error{ glGetError() };

		if (error != GL_NO_ERROR)
		{
			switch (error)
			{
				case GL_INVALID_ENUM:
				{
					ASSERT(false, "OpenGL call failed with 'GL_INVALID_ENUM'!");

					break;
				}

				case GL_INVALID_OPERATION:
				{
					ASSERT(false, "OpenGL call failed with 'GL_INVALID_OPERATION'!");

					break;
				}

				default:
				{
					ASSERT(false, "OpenGL call failed!");

					break;
				}
			}
		}
	}

	/*
	*	Retrieves all OpenGL function pointers.
	*/
	extern void RetrieveOpenGLFunctionPointers() NOEXCEPT;

}
#endif