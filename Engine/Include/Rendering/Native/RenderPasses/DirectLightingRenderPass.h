#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DirectLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VisibilityDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/VisibilityRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectLightingRenderPass);

	/*
	*	Default constructor.
	*/
	DirectLightingRenderPass() NOEXCEPT;

private:

	/*
	*	Light pipelines class definition.
	*/
	class LightPipelines final
	{

	public:

		//The visibility ray tracing pipeline.
		VisibilityRayTracingPipeline _VisibilityRayTracingPipeline;

		//The visibility denoising graphics pipelines.
		StaticArray<VisibilityDenoisingGraphicsPipeline, 2> _VisibilityDenoisingGraphicsPipelines;

		//The direct lighting application graphics pipeline.
		DirectLightingApplicationGraphicsPipeline _DirectLightingApplicationGraphicsPipeline;

	};

	//The lights pipelines.
	StaticArray<LightPipelines, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS> _LightsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};