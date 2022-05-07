#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class VolumetricLightingRayTracingPipeline final : public RayTracingPipeline
{

public:

	/*
	*	Initializes this ray tracing pipeline.
	*/
	void Initialize(const RenderTargetHandle target) NOEXCEPT;

	/*
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this ray tracing pipeline.
	*/
	void Terminate() NOEXCEPT;

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
	void CreateRenderDataTable(const RenderTargetHandle target) NOEXCEPT;

};