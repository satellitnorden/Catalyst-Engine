#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/ShadowUniformData.h>
#include <Rendering/Native/ShadowsSystem.h>
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/Core/RayTracingRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class ShadowsRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ShadowsRenderPass
	);

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
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap")),
		GraphicsRenderPipeline(HashString("ClearShadowMap"))
	};

	//The opaque model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _OpaqueModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap"))
	};

	//The masked model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA> _MaskedModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap"))
	};

	//The shadow map resolve pipeline.
	GraphicsRenderPipeline _ShadowMapResolvePipeline{ HashString("ShadowMapResolve") };

	//The ray traced shadows pipeline.
	RayTracingRenderPipeline _RayTracedShadowsPipeline{ HashString("RayTracedShadows") };

	//The shadows spatial denoising pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _ShadowsSpatialDenoisingPipelines
	{
		GraphicsRenderPipeline(HashString("ShadowsSpatialDenoising")),
		GraphicsRenderPipeline(HashString("ShadowsSpatialDenoising"))
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