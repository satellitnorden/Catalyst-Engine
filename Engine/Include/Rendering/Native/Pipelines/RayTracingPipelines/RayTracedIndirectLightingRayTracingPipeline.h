#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingConfiguration.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class RayTracedIndirectLightingRayTracingPipeline final : public RayTracingPipeline
{

public:

	/*
	*	Initializes this ray tracing pipeline.
	*/
	void Initialize(const RenderingConfiguration::IndirectLightingQuality quality) NOEXCEPT;

	/*
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this ray tracing pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The indirect lighting quality.
	RenderingConfiguration::IndirectLightingQuality _IndirectLightingQuality;

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