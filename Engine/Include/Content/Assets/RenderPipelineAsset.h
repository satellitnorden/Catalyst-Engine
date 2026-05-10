#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/Asset.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class RenderPipelineAsset final : public Asset
{

public:

	/*
	*	Common data class definition.
	*/
	class CommonData final
	{

	public:

		//The input stream subscriptions.
		DynamicArray<HashString> _InputStreamSubscriptions;

	};

	/*
	*	Graphics data class definition.
	*/
	class GraphicsData final
	{

	public:

		//The topology.
		Topology _Topology{ Topology::TriangleFan };

		//The cull mode.
		CullMode _CullMode{ CullMode::NONE };

		//The depth/stencil load operator.
		AttachmentLoadOperator _DepthStencilLoadOperator{ AttachmentLoadOperator::LOAD };

		//The depth/stencil store operator.
		AttachmentStoreOperator _DepthStencilStoreOperator{ AttachmentStoreOperator::STORE };

		//The color load operator.
		AttachmentLoadOperator _ColorLoadOperator{ AttachmentLoadOperator::LOAD };

		//The color store operator.
		AttachmentStoreOperator _ColorStoreOperator{ AttachmentStoreOperator::STORE };

		//Denotes whether or not the depth test is enabled.
		bool _DepthTestEnabled{ false };

		//Denotes whether or not the stencil test is enabled.
		bool _StencilTestEnabled{ false };

		//Denotes whether or not depth write is enabled.
		bool _DepthWriteEnabled{ false };

		//The depth compare operator.
		CompareOperator _DepthCompareOperator{ CompareOperator::Always };

		//The stencil compare operator.
		CompareOperator _StencilCompareOperator{ CompareOperator::Always };

		//The stencil fail operator.
		StencilOperator _StencilFailOperator{ StencilOperator::Keep };

		//The stencil pass operator.
		StencilOperator _StencilPassOperator{ StencilOperator::Keep };

		//The stencil depth fail operator.
		StencilOperator _StencilDepthFailOperator{ StencilOperator::Keep };

		//The stencil reference mask.
		uint32 _StencilReferenceMask{ 0 };

		//The stencil compare mask.
		uint32 _StencilCompareMask{ 0 };

		//The stencil write mask mask.
		uint32 _StencilWriteMask{ 0 };

		//The render resolution.
		HashString _RenderResolution;

		//The depth buffer.
		HashString _DepthBuffer;

		//Denotes whether or not blend is enabled.
		bool _BlendEnabled{ false };

		//The blend color source factor.
		BlendFactor _BlendColorSourceFactor{ BlendFactor::SOURCE_ALPHA };

		//The blend color destination factor.
		BlendFactor _BlendColorDestinationFactor{ BlendFactor::ONE_MINUS_SOURCE_ALPHA };

		//The blend color operator.
		BlendOperator _BlendColorOperator{ BlendOperator::ADD };

		//The blend alpha source factor.
		BlendFactor _BlendAlphaSourceFactor{ BlendFactor::ONE };

		//The blend alpha destination factor.
		BlendFactor _BlendAlphaDestinationFactor{ BlendFactor::ZERO };

		//The blend alpha operator.
		BlendOperator _BlendAlphaOperator{ BlendOperator::ADD };

	};

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The common data.
	CommonData _CommonData;

	//The graphics data.
	GraphicsData _GraphicsData;

};