#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
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

	//The ray traced diffuse irradiance pipeline.
	RayTracingRenderPipeline _RayTracedDiffuseIrradiancePipeline{ HashString("RayTracedDiffuseIrradiance_RenderPipeline") };

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