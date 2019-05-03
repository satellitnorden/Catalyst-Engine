#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class TemporalCompositingGraphicsPipeline final : public GraphicsPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TemporalCompositingGraphicsPipeline);

	/*
	*	Default constructor.
	*/
	TemporalCompositingGraphicsPipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The current index.
	uint8 _CurrentIndex{ 0 };

	/*
	*	Initializes the temporal compositing graphics pipeline.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

	/*
	*	Renders the temporal compositing.
	*/
	void RenderInternal() NOEXCEPT;

};