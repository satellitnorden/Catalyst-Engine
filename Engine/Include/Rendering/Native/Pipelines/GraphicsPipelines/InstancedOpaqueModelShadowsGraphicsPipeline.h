#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class InstancedOpaqueModelShadowsGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const bool double_sided, const DepthBufferHandle depth_buffer, const RenderTargetHandle render_target) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute(const uint8 cascade_index, const Matrix4x4 &world_to_light_matrix) NOEXCEPT;

private:

	//Denotes whether or not instanced models should be drawn double sided.
	bool _DoubleSided;

};