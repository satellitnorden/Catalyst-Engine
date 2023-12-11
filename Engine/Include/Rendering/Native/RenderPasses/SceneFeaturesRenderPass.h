#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/AnimatedModelSceneFeaturesGraphicsPipeline.h>
#if defined(CATALYST_EDITOR)
#include <Rendering/Native/Pipelines/GraphicsPipelines/EditorSelectedModelGraphicsPipeline.h>
#endif
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedImpostorColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/OpaqueModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/ParticleSystemComputePipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) SceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	SceneFeaturesRenderPass() NOEXCEPT;

private:

	//The particle system compute pipeline.
	ParticleSystemComputePipeline _ParticleSystemComputePipeline;

	//The graphics render pipelines 1.
	StaticArray<GraphicsRenderPipeline, 6> _GraphicsRenderPipelines1
	{
		GraphicsRenderPipeline(HashString("Clear_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelDepthSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelDepthDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedImpostorDepth_RenderPipeline"))
	};

	//The particle system masked depth graphics pipeline.
	ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline _ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline;

	//The graphics render pipelines 2.
	StaticArray<GraphicsRenderPipeline, 4> _GraphicsRenderPipelines2
	{
		GraphicsRenderPipeline(HashString("MaskedModelColorSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelColorDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelColorSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelColorDoubleSided_RenderPipeline"))
	};

	//The instanced impostor color scene features graphics pipeline.
	InstancedImpostorColorSceneFeaturesGraphicsPipeline _InstancedImpostorColorSceneFeaturesGraphicsPipeline;

	//The particle system masked color graphics pipeline.
	ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline _ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline;

	//The terrain scene features graphics pipeline.
	TerrainSceneFeaturesGraphicsPipeline _TerrainSceneFeaturesGraphicsPipeline;

	//The opaque model scene features graphics pipelines.
	StaticArray<OpaqueModelSceneFeaturesGraphicsPipeline, 2> _OpaqueModelSceneFeaturesGraphicsPipelines;

	//The animated model scene features graphics pipeline.
	AnimatedModelSceneFeaturesGraphicsPipeline _AnimatedModelSceneFeaturesGraphicsPipeline;

	//The user interface scene features graphics pipeline.
	UserInterfaceSceneFeaturesGraphicsPipeline _UserInterfaceSceneFeaturesGraphicsPipeline;

#if defined(CATALYST_EDITOR)
	//The editor selected model graphics pipeline.
	EditorSelectedModelGraphicsPipeline _EditorSelectedModelGraphicsPipeline;
#endif

	//The velocity graphics pipeline.
	GraphicsRenderPipeline _VelocityGraphicsPipeline{ HashString("Velocity_RenderPipeline") };

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