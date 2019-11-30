#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AmbientOcclusionDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceAmbientOcclusionGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class AmbientOcclusionRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(AmbientOcclusionRenderPass);

	/*
	*	Default constructor.
	*/
	AmbientOcclusionRenderPass() NOEXCEPT;

private:

	//The screen space ambient occlusion graphics pipeline.
	ScreenSpaceAmbientOcclusionGraphicsPipeline _ScreenSpaceAmbientOcclusionGraphicsPipeline;

	//The ambient occlusion denoising graphics pipelines.
	StaticArray<AmbientOcclusionDenoisingGraphicsPipeline, 2> _AmbientOcclusionDenoisingGraphicsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};