#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/PathTracingDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAccumulationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/PathTracingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class PathTracingRayTracingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(PathTracingRayTracingRenderPass);

	/*
	*	Default constructor.
	*/
	PathTracingRayTracingRenderPass() NOEXCEPT;

private:

	//The temporacl accumulation render targets.
	StaticArray<RenderTargetHandle, 2> _TemporalAccumulationRenderTargets;

	//The path tracing ray tracing pipeline.
	PathTracingRayTracingPipeline _PathTracingRayTracingPipeline;

	//The path tracing denoising graphics pipelines.
	StaticArray<PathTracingDenoisingGraphicsPipeline, 2> _PathTracingDenoisingGraphicsPipelines;

	//The temporal accumulation graphics pipelines.
	StaticArray<TemporalAccumulationGraphicsPipeline, 2> _TemporalAccumulationGraphicsPipeline;

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};