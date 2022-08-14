#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceIndirectLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracedIndirectLightingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) IndirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IndirectLightingRenderPass);

	/*
	*	Default constructor.
	*/
	IndirectLightingRenderPass() NOEXCEPT;

private:

	//The previous indirect lighting mode.
	RenderingConfiguration::IndirectLightingMode _PreviousIndirectLightingMode{ RenderingConfiguration::IndirectLightingMode::NONE };

	//The current indirect lighting mode.
	RenderingConfiguration::IndirectLightingMode _CurrentIndirectLightingMode{ RenderingConfiguration::IndirectLightingMode::NONE };

	//The previous indirect lighting quality.
	RenderingConfiguration::IndirectLightingQuality _PreviousIndirectLightingQuality{ RenderingConfiguration::IndirectLightingQuality::LOW };

	//The current indirect lighting quality.
	RenderingConfiguration::IndirectLightingQuality _CurrentIndirectLightingQuality{ RenderingConfiguration::IndirectLightingQuality::LOW };

	//The temporal indirect lighting buffers.
	StaticArray<RenderTargetHandle, 2> _TemporalIndirectLightingBuffers;

	//The screen space indirect lighting graphics pipeline.
	ScreenSpaceIndirectLightingGraphicsPipeline _ScreenSpaceIndirectLightingGraphicsPipeline;

	//The ray traced indirect lighting ray tracing pipeline.
	RayTracedIndirectLightingRayTracingPipeline _RayTracedIndirectLightingRayTracingPipeline;

	//The indirect lighting spatial denoising graphics pipelines.
	StaticArray<IndirectLightingSpatialDenoisingGraphicsPipeline, 2> _IndirectLightingSpatialDenoisingGraphicsPipelines;

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

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};