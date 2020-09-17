#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class ModelShadowMapGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const DepthBufferHandle depth_buffer, const RenderTargetHandle render_target) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute(const Matrix4x4 &world_to_light_matrix) NOEXCEPT;

};