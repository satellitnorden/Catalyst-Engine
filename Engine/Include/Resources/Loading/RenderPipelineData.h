#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

class RenderPipelineData final
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

	//The vertex shader data.
	ShaderData _VertexShaderData;

	//The fragment shader data.
	ShaderData _FragmentShaderData;

	//The input render targets.
	DynamicArray<HashString> _InputRenderTargets;

	//The output render targets.
	DynamicArray<HashString> _OutputRenderTargets;

};