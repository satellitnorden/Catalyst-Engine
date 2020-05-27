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
	void Initialize(const uint32 source_render_target_index_1,
					const uint32 source_render_target_index_2,
					const uint32 scene_features_4_render_target_index,
					const RenderTargetHandle target_1,
					const RenderTargetHandle target_2,
					const Resolution render_resolution) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The first source render target index.
	uint32 _SourceRenderTargetIndex1;

	//The second source render target index.
	uint32 _SourceRenderTargetIndex2;

	//The scene features 4 render target index.
	uint32 _SceneFeatures4RenderTargetIndex;

};