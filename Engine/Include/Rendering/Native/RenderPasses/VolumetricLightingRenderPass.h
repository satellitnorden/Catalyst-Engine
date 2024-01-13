#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/ComputeRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/RayTracingRenderPipeline.h>
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
	GraphicsRenderPipeline _VolumetricLightingGraphicsPipeline{ HashString("VolumetricLighting_RenderPipeline") };

	//The volumetric lighting ray tracing pipeline.
	RayTracingRenderPipeline _VolumetricLightingRayTracingPipeline{ HashString("RayTracedVolumetricLighting_RenderPipeline") };

	//The volumetric lighting tempora denoising pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _VolumetricLightingTemporalDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("VolumetricLightingTemporalDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("VolumetricLightingTemporalDenoising_RenderPipeline"))
	};

	//The volumetric lighting spatial denoising pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _VolumetricLightingSpatialDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("VolumetricLightingSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("VolumetricLightingSpatialDenoising_RenderPipeline"))
	};

	//The volumetric lighting application pipeline.
	ComputeRenderPipeline _VolumetricLightingApplicationPipeline{ HashString("VolumetricLightingApplication_RenderPipeline") };

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