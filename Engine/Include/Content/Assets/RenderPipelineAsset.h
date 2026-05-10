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

		//The render resolution.
		HashString _RenderResolution;

		//The depth buffer.
		HashString _DepthBuffer;

	};

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The common data.
	CommonData _CommonData;

	//The graphics data.
	GraphicsData _GraphicsData;

};