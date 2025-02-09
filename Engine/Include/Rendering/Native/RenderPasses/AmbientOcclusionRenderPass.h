#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/ComputeRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>
#include <Rendering/Native/RenderingConfiguration.h>

//Systems.
#include <Systems/System.h>

class AmbientOcclusionRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		AmbientOcclusionRenderPass
	);

	/*
	*	Default constructor.
	*/
	AmbientOcclusionRenderPass() NOEXCEPT;

private:

	//The current ambient occlusion mode.
	RenderingConfiguration::AmbientOcclusionMode _AmbientOcclusionMode{ RenderingConfiguration::AmbientOcclusionMode::NONE };

	//The ambient occlusion render target.
	RenderTargetHandle _AmbientOcclusionRenderTarget;

	//The intermediate ambient occlusion render target.
	RenderTargetHandle _IntermediateAmbientOcclusionRenderTarget;

	//The ambient occlusion temporal buffer render targets.
	StaticArray<RenderTargetHandle, 2> _AmbientOcclusionTemporalBufferRenderTargets;

	//The screen space ambient occlusion graphics pipeline.
	GraphicsRenderPipeline _ScreenSpaceAmbientOcclusionGraphicsPipeline{ HashString("ScreenSpaceAmbientOcclusion_RenderPipeline") };

	//The horizon based ambient occlusion graphics pipeline.
	GraphicsRenderPipeline _HorizonBasedAmbientOcclusionGraphicsPipeline{ HashString("HorizonBasedAmbientOcclusion_RenderPipeline") };

	//The ambient occlusion temporal denoising graphics pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _AmbientOcclusionTemporalDenoisingGraphicsPipelines
	{
		GraphicsRenderPipeline(HashString("AmbientOcclusionTemporalDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("AmbientOcclusionTemporalDenoising_RenderPipeline"))
	};

	//The ambient occlusion spatial denoising graphics pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _AmbientOcclusionSpatialDenoisingGraphicsPipelines
	{
		GraphicsRenderPipeline(HashString("AmbientOcclusionSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("AmbientOcclusionSpatialDenoising_RenderPipeline"))
	};

	//The ambient occlusion application graphics pipeline.
	ComputeRenderPipeline _AmbientOcclusionApplicationGraphicsPipeline{ HashString("AmbientOcclusionApplication_RenderPipeline") };

	//The current temporal buffer index.
	uint8 _CurrentTemporalBufferIndex{ 0 };

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