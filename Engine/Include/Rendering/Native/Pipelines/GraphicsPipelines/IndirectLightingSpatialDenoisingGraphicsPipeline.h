#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class IndirectLightingSpatialDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const uint32 source_render_target_index,
					const uint32 scene_features_2_render_target_index,
					const uint32 scene_features_3_render_target_index,
					const int32 stride,
					const RenderTargetHandle target,
					const Resolution render_resolution) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

	//The scene features 2 render target index.
	uint32 _SceneFeatures2RenderTargetIndex;

	//The scene features 3 render target index.
	uint32 _SceneFeatures3RenderTargetIndex;

	//The stride.
	int32 _Stride;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle source) NOEXCEPT;

};