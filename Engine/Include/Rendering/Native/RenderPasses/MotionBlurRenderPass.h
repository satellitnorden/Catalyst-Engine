#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/MotionBlurGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class MotionBlurRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(MotionBlurRenderPass);

	/*
	*	Default constructor.
	*/
	MotionBlurRenderPass() NOEXCEPT;

private:

	//The motion blur graphics pipeline.
	MotionBlurGraphicsPipeline _MotionBlurGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif