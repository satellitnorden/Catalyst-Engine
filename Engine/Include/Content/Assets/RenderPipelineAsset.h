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

		//The color load operator.
		AttachmentLoadOperator _ColorLoadOperator{ AttachmentLoadOperator::DONT_CARE };

		//The color store operator.
		AttachmentStoreOperator _ColorStoreOperator{ AttachmentStoreOperator::DONT_CARE };

		//The render resolution.
		HashString _RenderResolution;

	};

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The common data.
	CommonData _CommonData;

	//The graphics data.
	GraphicsData _GraphicsData;

};