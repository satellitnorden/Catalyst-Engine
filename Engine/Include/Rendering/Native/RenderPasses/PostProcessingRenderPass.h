#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class PostProcessingRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		PostProcessingRenderPass
	);

	/*
	*	Default constructor.
	*/
	PostProcessingRenderPass() NOEXCEPT;

private:

	//The motion blur graphics pipeline.
	GraphicsRenderPipeline _MotionBlurPipeline{ HashString("MotionBlur_RenderPipeline") };

	//The post process pipeline.
	GraphicsRenderPipeline _PostProcessPipeline{ HashString("PostProcess_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};