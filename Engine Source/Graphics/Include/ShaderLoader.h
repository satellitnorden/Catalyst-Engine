#pragma once

//Engine core.
#include <EngineCore.h>

//Preprocessor defines.
#define SHADERS_PATH "../../Engine Source/Graphics/Shaders/"

namespace ShaderLoader
{
	
	/*
	*	Given a shader name, returns a vector of bytes.
	*/
	DynamicArray<char> LoadShader(const char *RESTRICT shaderName) NOEXCEPT
	{
		DynamicString fullShaderName{ SHADERS_PATH };
		fullShaderName += shaderName;

		std::ifstream file(fullShaderName.CString(), std::ios::ate | std::ios::binary);

#if !defined(CATALYST_FINAL)
		if (!file.is_open())
			BREAKPOINT;
#endif

		size_t fileSize = (size_t)file.tellg();
		DynamicArray<char> buffer;
		buffer.Resize(fileSize);

		file.seekg(0);
		file.read(buffer.Data(), fileSize);

		file.close();

		return buffer;
	}

}