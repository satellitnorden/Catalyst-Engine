#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class VisibilityRayTracingPipeline final : public RayTracingPipeline
{

public:

	/*
	*	Initializes this ray tracing pipeline.
	*/
	void Initialize(const int32 lightIndex) NOEXCEPT;

	/*
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The light index.
	int32 _LightIndex;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};