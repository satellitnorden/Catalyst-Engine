#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class CompositingComputePipeline final : public ComputePipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CompositingComputePipeline);

	/*
	*	Default constructor.
	*/
	CompositingComputePipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes this compute pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Executes this compute pipeline.
	*/
	void Execute() NOEXCEPT;

};