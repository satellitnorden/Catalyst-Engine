#if defined(CATALYST_HAS_OPEN_GL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#if defined(CATALYST_PLATFORM_WINDOWS)
#include <Platform/Windows/CatalystPlatformWindows.h>
#endif

//Third party.
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>

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

//List of all function pointers.
#define OPEN_GL_FUNCTIONS											\
OPEN_GL_FUNCTION(PFNGLACTIVETEXTUREPROC, glActiveTexture)			\
OPEN_GL_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader)				\
OPEN_GL_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)		\
OPEN_GL_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader)			\
OPEN_GL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram)			\
OPEN_GL_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader)				\
OPEN_GL_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader)				\
OPEN_GL_FUNCTION(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap)			\
OPEN_GL_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)		\
OPEN_GL_FUNCTION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)	\
OPEN_GL_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv)				\
OPEN_GL_FUNCTION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)		\
OPEN_GL_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv)				\
OPEN_GL_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)	\
OPEN_GL_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram)				\
OPEN_GL_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource)				\
OPEN_GL_FUNCTION(PFNGLUNIFORM1FPROC, glUniform1f)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM1UIPROC, glUniform1ui)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM2FPROC, glUniform2f)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM2FVPROC, glUniform2fv)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM4FPROC, glUniform4f)					\
OPEN_GL_FUNCTION(PFNGLUNIFORM4FVPROC, glUniform4fv)					\
OPEN_GL_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram)					\

#define OPEN_GL_FUNCTION(TYPE, NAME) extern TYPE NAME;
OPEN_GL_FUNCTIONS
#undef OPEN_GL_FUNCTION
#endif