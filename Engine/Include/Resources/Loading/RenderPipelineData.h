#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

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

	//The output depth buffer.
	HashString _OutputDepthBuffer;

	//The output render targets.
	DynamicArray<HashString> _OutputRenderTargets;

	//The color load operator.
	AttachmentLoadOperator _ColorLoadOperator;

	//The color store operator.
	AttachmentStoreOperator _ColorStoreOperator;

	//The depth/stencil load operator.
	AttachmentLoadOperator _DepthStencilLoadOperator;

	//The depth/stencil store operator.
	AttachmentStoreOperator _DepthStencilStoreOperator;

	//Denotes whether or not blend is enabled.
	bool _BlendEnabled{ false };

	//The blend color source factor.
	BlendFactor _BlendColorSourceFactor{ BlendFactor::SourceAlpha };

	//The blend color destination factor.
	BlendFactor _BlendColorDestinationFactor{ BlendFactor::OneMinusSourceAlpha };

	//The blend color operator.
	BlendOperator _BlendColorOperator{ BlendOperator::ADD };

	//The blend alpha source factor.
	BlendFactor _BlendAlphaSourceFactor{ BlendFactor::SourceAlpha };

	//The blend alpha destination factor.
	BlendFactor _BlendAlphaDestinationFactor{ BlendFactor::OneMinusSourceAlpha };

	//The blend alpha operator.
	BlendOperator _BlendAlphaOperator{ BlendOperator::ADD };

};