#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Resources.
#include <Resources/Core/RenderPipelineResource.h>

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
	void Initialize() NOEXCEPT;

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

	//The render pipeline resource.
	ResourcePointer<RenderPipelineResource> _RenderPipelineResource;

	//Denotes if this render pipeline uses it's own render data table.
	bool _UsesRenderDataTable;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

};