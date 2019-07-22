#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/VolumetricLightingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class VolumetricLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(VolumetricLightingRenderPass);

	/*
	*	Default constructor.
	*/
	VolumetricLightingRenderPass() NOEXCEPT;

private:

	//The volumetric lighting graphics pipeline.
	VolumetricLightingGraphicsPipeline _VolumetricLightingGraphicsPipeline;

	//The volumetric lighting ray tracing pipeline.
	VolumetricLightingRayTracingPipeline _VolumetricLightingRayTracingPipeline;

	//The volumetric lighting denoising graphics pipelines.
	StaticArray<VolumetricLightingDenoisingGraphicsPipeline, 2> _VolumetricLightingDenoisingGraphicsPipelines;

	//The volumetric lighting application graphics pipeline.
	VolumetricLightingApplicationGraphicsPipeline _VolumetricLightingApplicationGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};