#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class WorldRayTracingPipeline final : public RayTracingPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldRayTracingPipeline);

	/*
	*	Default constructor.
	*/
	WorldRayTracingPipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes this ray tracing pipeline.
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
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

};