#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/SamplerProperties.h>

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

	//The included uniform buffers.
	DynamicArray<HashString> _IncludedUniformBuffers;

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

	//The cull mode.
	CullMode _CullMode;

	//Denotes whether or not depth test is enabled.
	bool _DepthTestEnabled;

	//Denotes whether or not depth write is enabled.
	bool _DepthWriteEnabled;

	//The depth compare operator.
	CompareOperator _DepthCompareOperator;

	//Denotes whether or not stencil test is enabled.
	bool _StencilTestEnabled;

	//The action performed on samples that fail the stencil test.
	StencilOperator _StencilFailOperator;

	//The action performed on samples that pass both the depth test and the stencil test.
	StencilOperator _StencilPassOperator;

	//The action performed on samples that fail the depth test but pass the stencil test.
	StencilOperator _StencilDepthFailOperator;

	//The stencil compare operator.
	CompareOperator _StencilCompareOperator;

	//The stencil compare mask.
	uint32 _StencilCompareMask;

	//The stencil write mask.
	uint32 _StencilWriteMask;

	//The stencil reference mask.
	uint32 _StencilReferenceMask;

	//The topology.
	Topology _Topology;

	//The sampler properties.
	DynamicArray<SamplerProperties> _SamplerProperties;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

};