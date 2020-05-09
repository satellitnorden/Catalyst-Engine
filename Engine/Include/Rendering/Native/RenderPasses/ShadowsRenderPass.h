#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelShadowMapGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/RasterizedShadowsGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ShadowsSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/ShadowsRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ShadowsRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ShadowsRenderPass);

	/*
	*	Default constructor.
	*/
	ShadowsRenderPass() NOEXCEPT;

private:

	//The shadow map depth buffer.
	DepthBufferHandle _ShadowMapDepthBuffer;

	//The model shadow map graphics pipeline.
	ModelShadowMapGraphicsPipeline _ModelShadowMapGraphicsPipeline;

	//The rasterized shadows graphics pipeline.
	RasterizedShadowsGraphicsPipeline _RasterizedShadowsGraphicsPipeline;

	//The shadows ray tracing pipeline.
	ShadowsRayTracingPipeline _ShadowsRayTracingPipeline;

	//The shadows spatial denoising graphics pipelines.
	StaticArray<ShadowsSpatialDenoisingGraphicsPipeline, 2> _ShadowsSpatialDenoisingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};