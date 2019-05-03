#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class SecondTemporalAccumulationGraphicsPipeline final : public GraphicsPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SecondTemporalAccumulationGraphicsPipeline);

	/*
	*	Default constructor.
	*/
	SecondTemporalAccumulationGraphicsPipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The current index.
	uint8 _CurrentIndex{ 0 };

	/*
	*	Initializes the second temporal accumulation graphics pipeline.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the second temporal accumulation.
	*/
	void RenderInternal() NOEXCEPT;

};