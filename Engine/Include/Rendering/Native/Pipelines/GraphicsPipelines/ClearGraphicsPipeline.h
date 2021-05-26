#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class ClearGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(
#if !defined(CATALYST_SIMPLIFIED_RENDERING)
		const DepthBufferHandle depth_buffer
#endif
	) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

};