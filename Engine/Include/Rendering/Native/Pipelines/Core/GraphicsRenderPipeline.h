#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>
#include <Core/General/Pair.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Resources.
#include <Resources/Core/RenderPipelineResource.h>

class GraphicsRenderPipelineInitializeParameters final
{

public:

	//The depth buffer.
	Optional<Pair<HashString, DepthBufferHandle>> _DepthBuffer;

	//The input render targets.
	DynamicArray<Pair<HashString, RenderTargetHandle>> _InputRenderTargets;

	//The output render targets.
	DynamicArray<Pair<HashString, RenderTargetHandle>> _OutputRenderTargets;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

	//Denotes whether or not to enable depth test.
	Optional<bool> _DepthTestEnabled;

};

class GraphicsRenderPipelineExecuteParameters final
{

public:

	/*
	*	The push constant data.
	*	If set, will override all other push constant data.
	*/
	void *RESTRICT _PushConstantData{ nullptr };

};

class GraphicsRenderPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE GraphicsRenderPipeline(const HashString render_pipeline_identifier) NOEXCEPT
		:
		_RenderPipelineIdentifier(render_pipeline_identifier)
	{

	}

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const GraphicsRenderPipelineInitializeParameters &parameters = GraphicsRenderPipelineInitializeParameters()) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute(const GraphicsRenderPipelineExecuteParameters *const RESTRICT parameters = nullptr) NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The render pipeline identifier.
	HashString _RenderPipelineIdentifier;

	//The render pipeline resource.
	ResourcePointer<RenderPipelineResource> _RenderPipelineResource;

	//Denotes if this render pipeline uses it's own render data table.
	bool _UsesRenderDataTable;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The storage buffer includes.
	DynamicArray<Pair<HashString, uint32>> _StorageBufferIncludes;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

};