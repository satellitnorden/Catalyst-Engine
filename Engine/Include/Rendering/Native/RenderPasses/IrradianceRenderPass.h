#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/RayTracingRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) IrradianceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IrradianceRenderPass);

	/*
	*	Default constructor.
	*/
	IrradianceRenderPass() NOEXCEPT;

private:

	//The intermediate diffuse irradiance render target.
	RenderTargetHandle _IntermediateDiffuseIrradianceRenderTarget;

	//The diffuse irradiance temporal buffers.
	StaticArray<RenderTargetHandle, 2> _DiffuseIrradianceTemporalBuffers;

	//The current temporal buffer index.
	uint8 _CurrentTemporalBufferIndex{ 0 };

	//The ray traced diffuse irradiance pipeline.
	RayTracingRenderPipeline _RayTracedDiffuseIrradiancePipeline{ HashString("RayTracedDiffuseIrradiance_RenderPipeline") };

	//The diffuse irradiance temporal denoising pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _DiffuseIrradianceTemporalDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("DiffuseIrradianceTemporalDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceTemporalDenoising_RenderPipeline"))
	};

	//The passthrough pipeline.
	GraphicsRenderPipeline _PassthroughPipeline{ HashString("PassthroughHalf_RenderPipeline") };

	//The diffuse irradiance spatial denoising pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _DiffuseIrradianceSpatialDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline"))
	};

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