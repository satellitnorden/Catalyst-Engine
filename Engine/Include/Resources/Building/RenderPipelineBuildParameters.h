#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

class RenderPipelineBuildParameters final
{

public:

	/*
	*	Shader data class definition.
	*/
	class ShaderData final
	{

	public:

		//The GLSL data.
		DynamicArray<byte> _GLSLData;

	};

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The vertex shader data.
	ShaderData _VertexShaderData;

	//The fragment shader data.
	ShaderData _FragmentShaderData;

	//The input render targets.
	DynamicArray<HashString> _InputRenderTargets;

	//The output render targets.
	DynamicArray<HashString> _OutputRenderTargets;

};