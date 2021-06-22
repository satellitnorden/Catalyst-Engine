#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class AmbientOcclusionSpatialDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const uint32 source_render_target_index, const int32 stride, const RenderTargetHandle target) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline-
	*/
	void Terminate() NOEXCEPT;

private:

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

	//The stride.
	int32 _Stride;

};