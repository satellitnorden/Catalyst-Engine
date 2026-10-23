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
	namespace Texture
	{

		/*
		*	Create a texture with the given parameters.
		*	Returns if it succeeded.
		*/
		FORCE_INLINE NO_DISCARD bool Create
		(
			OpenGL::Identifier *const RESTRICT identifier,
			const uint32 width,
			const uint32 height,
			const GLuint format,
			const void *const RESTRICT data
		) NOEXCEPT
		{
			//Set the pixel unpack alignment to different values depending on the format.
			switch (format)
			{
				case GL_RED:
				{
					OPEN_GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

					break;
				}

				case GL_RGBA:
				{
					OPEN_GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Create the texture.
			OPEN_GL_CALL(glGenTextures(1, identifier));

			//Bind the texture.
			OPEN_GL_CALL(glBindTexture(GL_TEXTURE_2D, *identifier));

			//Set the texture parameters.
			OPEN_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			OPEN_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			OPEN_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			OPEN_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			//Store the data in the texture.
			OPEN_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));

			//Generate the mipmap.
			OPEN_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			return true;
		}

	}
}
#endif