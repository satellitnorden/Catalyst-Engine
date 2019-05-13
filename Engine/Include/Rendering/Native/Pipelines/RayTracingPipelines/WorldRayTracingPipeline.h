#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class WorldRayTracingPipeline final : public RayTracingPipeline
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

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The top level acceleration structure.
	AccelerationStructureHandle _TopLevelAccelerationStructure;

	//The blue noise textures.
	DynamicArray<Texture2DHandle> _BlueNoiseTextures;

	//The current blue noise texture index.
	uint8 _CurrentBlueNoiseTextureIndex{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

};