#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class IndirectLightingTemporalDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:
	
	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const uint32 source_render_target_index,
					const uint32 scene_features_4_render_target_index,
					const RenderTargetHandle previous_temporal_buffer,
					const RenderTargetHandle target_1,
					const RenderTargetHandle target_2,
					const Resolution render_resolution) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout{ EMPTY_HANDLE };

	//The render data table.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

	//The scene features 4 render target index.
	uint32 _SceneFeatures4RenderTargetIndex;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle previous_temporal_buffer) NOEXCEPT;

};