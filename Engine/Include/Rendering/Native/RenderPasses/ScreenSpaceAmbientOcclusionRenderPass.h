#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceAmbientOcclusionGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ScreenSpaceAmbientOcclusionRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenSpaceAmbientOcclusionRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenSpaceAmbientOcclusionRenderPass() NOEXCEPT;

private:

	//The screen space ambient occlusion graphics pipeline.
	ScreenSpaceAmbientOcclusionGraphicsPipeline _ScreenSpaceAmbientOcclusionGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};