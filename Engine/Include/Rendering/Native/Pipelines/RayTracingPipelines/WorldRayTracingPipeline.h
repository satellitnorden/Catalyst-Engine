#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

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

	//The number of noise textures.
	static constexpr uint8 NUMBER_OF_NOISE_TEXTURES{ 64 };

	//The noise texture size.
	static constexpr uint8 NOISE_TEXTURE_SIZE{ 64 };

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The top level acceleration structure.
	AccelerationStructureHandle _TopLevelAccelerationStructure;

	//The noise textures.
	StaticArray<Texture2DHandle, NUMBER_OF_NOISE_TEXTURES> _NoiseTextures;

	//The current noise texture index.
	uint8 _CurrentNoiseTextureIndex{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

	/*
	*	Creates the noise textures.
	*/
	void CreateNoiseTextures() NOEXCEPT;

};