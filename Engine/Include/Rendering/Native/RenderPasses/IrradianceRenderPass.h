#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/RayTracingRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class IrradianceRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		IrradianceRenderPass
	);

	/*
	*	Default constructor.
	*/
	IrradianceRenderPass() NOEXCEPT;

private:

	////////////////////////
	// DIFFUSE IRRADIANCE //
	////////////////////////

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
	StaticArray<GraphicsRenderPipeline, 6> _DiffuseIrradianceSpatialDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DiffuseIrradianceSpatialDenoising_RenderPipeline"))
	};

	//The screen space specular irradiance data render target
	RenderTargetHandle _ScreenSpaceSpecularIrradianceDataRenderTarget;

	//The screen space specular irradiance pipeline.
	GraphicsRenderPipeline _ScreenSpaceSpecularIrradiance{ HashString("ScreenSpaceSpecularIrradiance_RenderPipeline") };

	//The screen space specular irradiance resolve pipeline.
	GraphicsRenderPipeline _ScreenSpaceSpecularIrradianceResolve{ HashString("ScreenSpaceSpecularIrradianceResolve_RenderPipeline") };

	//The ray traced specular irradiance pipeline.
	RayTracingRenderPipeline _RayTracedSpecularIrradiancePipeline{ HashString("RayTracedSpecularIrradiance_RenderPipeline") };

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