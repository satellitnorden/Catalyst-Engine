#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceIndirectLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/IndirectLightingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class IndirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IndirectLightingRenderPass);

	/*
	*	Default constructor.
	*/
	IndirectLightingRenderPass() NOEXCEPT;

private:

	//Define constants.
	constexpr static uint8 NUMBER_OF_SPATIAL_DENOISING_PASSES{ 2 };

	//The screen space indirect lighting graphics pipelines.
	StaticArray<ScreenSpaceIndirectLightingGraphicsPipeline, 2> _ScreenSpaceIndirectLightingGraphicsPipelines;

	//The indirect lighting ray tracing pipeline.
	IndirectLightingRayTracingPipeline _IndirectLightingRayTracingPipeline;

	//The indirect lighting spatial denoising graphics pipelines.
	StaticArray<IndirectLightingSpatialDenoisingGraphicsPipeline, NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 * 2> _IndirectLightingSpatialDenoisingGraphicsPipelines;

	//The indirect lighting temporal denoising graphics pipelines.
	StaticArray<IndirectLightingTemporalDenoisingGraphicsPipeline, 4> _IndirectLightingTemporalDenoisingGraphicsPipelines;

	//The current temporal buffer index.
	uint8 _CurrentTemporalBufferIndex{ 0 };

	//The indirect lighting application graphics pipeline.
	IndirectLightingApplicationGraphicsPipeline _IndirectLightingApplicationGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};