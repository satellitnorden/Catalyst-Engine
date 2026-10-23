#if defined(CATALYST_HAS_OPEN_GL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Systems.
#include <Systems/LogSystem.h>

namespace OpenGL
{
	namespace Shader
	{

		/*
		*	Creates a shader with the given source.
		*	Returns if it succeeded.
		*/
		FORCE_INLINE NO_DISCARD bool Create(OpenGL::Identifier *const RESTRICT identifier, const GLenum type, const char *const RESTRICT source) NOEXCEPT
		{
			*identifier = glCreateShader(type);
			glShaderSource(*identifier, 1, &source, nullptr);
			glCompileShader(*identifier);

			int32 success;

			glGetShaderiv(*identifier, GL_COMPILE_STATUS, &success);

			if (success != 0)
			{
				return true;
			}

			else
			{
				char info_log[512];
				glGetShaderInfoLog(*identifier, ARRAY_LENGTH(info_log), nullptr, info_log);
				LOG_ERROR(info_log);

				return false;
			}
		}

	}
}
#endif