#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/ShadowUniformData.h>
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedOpaqueModelShadowsGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/RasterizedShadowsGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ShadowsSpatialDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainShadowMapGraphicsPipeline.h>
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

	//The shadow map depth buffers.
	StaticArray<DepthBufferHandle, 4> _ShadowMapDepthBuffers;

	//The shadow map render targets
	StaticArray<RenderTargetHandle, 4> _ShadowMapRenderTargets;

	//The shadow map render target indices.
	StaticArray<uint32, 4> _ShadowMapRenderTargetIndices;

	//The shadow uniform data.
	DynamicArray<ShadowUniformDataOld> _ShadowUniformData;

	//The shadow uniform data buffers.
	DynamicArray<BufferHandle> _ShadowUniformDataBuffers;

	//The shadow uniform data render data tables.
	DynamicArray<RenderDataTableHandle> _ShadowUniformDataRenderDataTables;

	//The clear shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _ClearShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ClearShadowMap_RenderPipeline"))
	};

	//The opaque model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _OpaqueModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelShadowMap_RenderPipeline"))
	};

	//The masked model shadow map pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _MaskedModelShadowMapPipelines
	{
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelShadowMap_RenderPipeline"))
	};

	//The terrain shadow graphics pipelines.
	StaticArray<TerrainShadowMapGraphicsPipeline, 4> _TerrainShadowMapGraphicsPipelines;

	//The instanced opaque model shadows graphics pipelines.
	StaticArray<InstancedOpaqueModelShadowsGraphicsPipeline, 8> _InstancedOpaqueModelShadowsGraphicsPipelines;

	//The rasterized shadows graphics pipeline.
	RasterizedShadowsGraphicsPipeline _RasterizedShadowsGraphicsPipeline;

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