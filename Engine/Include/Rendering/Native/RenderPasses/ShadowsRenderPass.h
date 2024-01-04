#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/ShadowUniformData.h>
#include <Rendering/Native/ShadowsSystem.h>
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ShadowsSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/ShadowsRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) ShadowsRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ShadowsRenderPass);

	/*
	*	Default constructor.
	*/
	ShadowsRenderPass() NOEXCEPT;

private:

	//Denotes if the shadow map pipelines is initialized.
	StaticArray<bool, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _ShadowMapPipelinesIsInitialized;

	//The clear shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _ClearShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline"))
	};

	//The opaque model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _OpaqueModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline"))
	};

	//The masked model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _MaskedModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline"))
	};

	//The shadow map resolve pipeline.
	GraphicsRenderPipeline _ShadowMapResolvePipeline{ HashString("ShadowMapResolve_RenderPipeline") };

	//The shadows ray tracing pipeline.
	ShadowsRayTracingPipeline _ShadowsRayTracingPipeline;

	//The shadows spatial denoising graphics pipelines.
	StaticArray<ShadowsSpatialDenoisingGraphicsPipeline, 2> _ShadowsSpatialDenoisingGraphicsPipelines;

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