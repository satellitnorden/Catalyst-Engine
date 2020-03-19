#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AmbientOcclusionApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/AmbientOcclusionSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/AmbientOcclusionTemporalDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceAmbientOcclusionGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/AmbientOcclusionRayTracingPipeline.h>
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

	//The ambient occlusion ray tracing pipeline.
	AmbientOcclusionRayTracingPipeline _AmbientOcclusionRayTracingPipeline;

	//The ambient occlusion spatial denoising graphics pipelines.
	StaticArray<AmbientOcclusionSpatialDenoisingGraphicsPipeline, 4> _AmbientOcclusionSpatialDenoisingGraphicsPipelines;

	//The ambient occlusion temporal denoising graphics pipelines.
	StaticArray<AmbientOcclusionTemporalDenoisingGraphicsPipeline, 2> _AmbientOcclusionTemporalDenoisingGraphicsPipelines;

	//The ambient occlusion application graphics pipeline.
	AmbientOcclusionApplicationGraphicsPipeline _AmbientOcclusionApplicationGraphicsPipeline;

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

};