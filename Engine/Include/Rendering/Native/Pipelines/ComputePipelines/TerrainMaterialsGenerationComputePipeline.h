#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class TerrainMaterialsGenerationComputePipeline final : public ComputePipeline
{

public:

	/*
	*	Initializes this compute pipeline.
	*/
	void Initialize(const ResourcePointer<ShaderResource> shader) NOEXCEPT;

	/*
	*	Executes this compute pipeline.
	*/
	void Execute(	const Vector2<float32> minimum_bounds,
					const Vector2<float32> maximum_bounds,
					const uint32 dimensions,
					const Texture2DHandle normal_texture,
					const Texture2DHandle indices_texture,
					const Texture2DHandle blend_texture,
					CommandBuffer *const RESTRICT command_buffer) NOEXCEPT;

	/*
	*	Destroys the render data tables.
	*/
	void DestroyRenderDataTables() NOEXCEPT;

	/*
	*	Returns if this pipeline is initialized.
	*/
	FORCE_INLINE NO_DISCARD bool IsInitialized() const NOEXCEPT
	{
		return _Initialized;
	}

private:

	//Denotes if this pipeline is initialized.
	bool _Initialized{ false };

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

};