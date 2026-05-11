#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Pair.h>

//Content.
#if defined(USE_RENDER_PIPELINE_ASSET)
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/RenderPipelineAsset.h>
#endif

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

//Resources.
#if !defined(USE_RENDER_PIPELINE_ASSET)
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/RenderPipelineResource.h>
#endif

class ComputeRenderPipelineParameters final
{

public:

	//The compute render targets.
	DynamicArray<Pair<HashString, RenderTargetHandle>> _ComputeRenderTargets;

};

class ComputeRenderPipeline final : public ComputePipeline
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ComputeRenderPipeline(const HashString render_pipeline_identifier) NOEXCEPT
		:
		_RenderPipelineIdentifier(render_pipeline_identifier)
	{

	}

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const ComputeRenderPipelineParameters &parameters = ComputeRenderPipelineParameters()) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The render pipeline identifier.
	HashString _RenderPipelineIdentifier;

#if defined(USE_RENDER_PIPELINE_ASSET)
	//The render pipeline asset.
	AssetPointer<RenderPipelineAsset> _RenderPipelineAsset;
#else
	//The render pipeline resource.
	ResourcePointer<RenderPipelineResource> _RenderPipelineResource;
#endif

	//Denotes if this render pipeline uses it's own render data table.
	bool _UsesRenderDataTable;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The compute render targets.
	DynamicArray<RenderTargetHandle> _ComputeRenderTargets;

};