#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>
#include <Core/General/Pair.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/SamplerProperties.h>

//Resources.
#include <Resources/Core/Resource.h>

class RenderPipelineResource final : public Resource
{

public:

	/*
	*	Ray hit group shader data class definition.
	*/
	class RayHitGroupShaderData final
	{

	public:

		//The identifier.
		HashString _Identifier;

		//The ray closest hit shader handle
		ShaderHandle _RayClosestHitShaderHandle{ EMPTY_HANDLE };

		//The ray any hit shader handle
		ShaderHandle _RayAnyHitShaderHandle{ EMPTY_HANDLE };

	};

	//The compute shader handle.
	ShaderHandle _ComputeShaderHandle{ EMPTY_HANDLE };

	//The vertex shader handle.
	ShaderHandle _VertexShaderHandle{ EMPTY_HANDLE };

	//The fragment shader handle.
	ShaderHandle _FragmentShaderHandle{ EMPTY_HANDLE };

	//The ray generation shader handle.
	ShaderHandle _RayGenerationShaderHandle{ EMPTY_HANDLE };

	//The ray miss shader handles.
	DynamicArray<ShaderHandle> _RayMissShaderHandles;

	//The ray hit group shader data.
	DynamicArray<RayHitGroupShaderData> _RayHitGroupShaderData;

	//The included uniform buffers.
	DynamicArray<HashString> _IncludedUniformBuffers;

	//The included storage buffers.
	DynamicArray<HashString> _IncludedStorageBuffers;

	//The compute render targets.
	DynamicArray<HashString> _ComputeRenderTargets;

	//The input render targets.
	DynamicArray<Pair<HashString, SamplerProperties>> _InputRenderTargets;

	//The output render targets.
	DynamicArray<HashString> _OutputRenderTargets;

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

	//The number of external textures.
	uint32 _NumberOfExternalTextures;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

};