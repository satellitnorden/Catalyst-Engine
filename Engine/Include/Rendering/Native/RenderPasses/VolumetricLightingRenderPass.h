#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingTemporalDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/VolumetricLightingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) VolumetricLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(VolumetricLightingRenderPass);

	/*
	*	Default constructor.
	*/
	VolumetricLightingRenderPass() NOEXCEPT;

private:

	//The volumetric lighting render target.
	RenderTargetHandle _VolumetricLightingRenderTarget;

	//The intermediate volumetric lighting render target.
	RenderTargetHandle _IntermediateVolumetricLightingRenderTarget;

	//The volumetric lighting temporal buffer render targets.
	StaticArray<RenderTargetHandle, 2> _VolumetricLightingTemporalBufferRenderTargets;

	//The volumetric lighting graphics pipeline.
	VolumetricLightingGraphicsPipeline _VolumetricLightingGraphicsPipeline;

	//The volumetric lighting ray tracing pipeline.
	VolumetricLightingRayTracingPipeline _VolumetricLightingRayTracingPipeline;

	//The volumetric lighting spatial denoising graphics pipelines.
	StaticArray<VolumetricLightingSpatialDenoisingGraphicsPipeline, 2> _VolumetricLightingSpatialDenoisingGraphicsPipelines;

	//The volumetric lighting temporal denoising graphics pipelines.
	StaticArray<VolumetricLightingTemporalDenoisingGraphicsPipeline, 2> _VolumetricLightingTemporalDenoisingGraphicsPipelines;

	//The volumetric lighting application graphics pipeline.
	VolumetricLightingApplicationGraphicsPipeline _VolumetricLightingApplicationGraphicsPipeline;

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