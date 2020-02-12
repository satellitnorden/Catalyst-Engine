#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class SkyComputePipeline final : public ComputePipeline
{

public:

	/*
	*	Initializes this compute pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this compute pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//Keeps track of the current iteration.
	uint32 _CurrentIteration{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Calculates the parameters difference.
	*/
	NO_DISCARD float CalculateParametersDifference() NOEXCEPT;

};