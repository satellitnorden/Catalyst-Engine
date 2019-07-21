#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DirectLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/DirectLightingRayTracingPipeline.h>
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

	//The direct lighting graphics pipeline.
	DirectLightingGraphicsPipeline _DirectLightingGraphicsPipeline;

	//The direct lighting ray tracing pipeline.
	DirectLightingRayTracingPipeline _DirectLightingRayTracingPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};