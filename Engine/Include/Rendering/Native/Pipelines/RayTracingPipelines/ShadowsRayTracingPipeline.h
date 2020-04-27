#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class ShadowsRayTracingPipeline final : public RayTracingPipeline
{

public:

	/*
	*	Initializes this ray tracing pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};