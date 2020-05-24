#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const DepthBufferHandle depth_buffer, const bool depth_test, const bool wireframe) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//Denotes whether or not to perform depth test.
	bool _DepthTest;

	//Denotes whether or not to draw as wireframe.
	bool _Wireframe;

};
#endif