#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

namespace ShaderLoader
{
	
	/*
	*	Given a shader name, returns a vector of bytes.
	*/
	static DynamicArray<char> LoadShader(const char *RESTRICT shaderName) NOEXCEPT
	{
		std::ifstream file(shaderName, std::ios::ate | std::ios::binary);

#if !defined(CATALYST_FINAL)
		if (!file.is_open())
			BREAKPOINT;
#endif

		uint64 fileSize = StaticCast<uint64>(file.tellg());
		DynamicArray<char> buffer;
		buffer.UpsizeFast(fileSize);

		file.seekg(0);
		file.read(buffer.Data(), fileSize);

		file.close();

		return buffer;
	}

}