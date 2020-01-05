#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>
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

	//The indirect lighting graphics pipeline.
	IndirectLightingGraphicsPipeline _IndirectLightingGraphicsPipeline;

	//The indirect lighting ray tracing pipeline.
	IndirectLightingRayTracingPipeline _IndirectLightingRayTracingPipeline;

	//The indirect lighting denoising graphics pipelines.
	StaticArray<IndirectLightingDenoisingGraphicsPipeline, 2> _IndirectLightingDenoisingGraphicsPipelines;

	//The indirect lighting temporal denoising graphics pipelines.
	StaticArray<IndirectLightingTemporalDenoisingGraphicsPipeline, 2> _IndirectLightingTemporalDenoisingGraphicsPipelines;

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