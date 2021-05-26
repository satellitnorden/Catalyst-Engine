#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AnimatedModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ClearGraphicsPipeline.h>
#if defined(CATALYST_EDITOR)
#include <Rendering/Native/Pipelines/GraphicsPipelines/EditorSelectedModelGraphicsPipeline.h>
#endif
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedImpostorColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedImpostorDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedStaticModelColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedStaticModelDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/MaskedModelColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/MaskedModelDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/OpaqueModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/ParticleSystemComputePipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ProceduralGrassSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VelocityGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class SceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	SceneFeaturesRenderPass() NOEXCEPT;

	/*
	*	Returns the scene depth buffer.
	*/
	FORCE_INLINE NO_DISCARD DepthBufferHandle GetSceneDepthBuffer() NOEXCEPT
	{
		return _SceneDepthBuffer;
	}

private:

	//The scene depth buffer.
	DepthBufferHandle _SceneDepthBuffer;

	//The clear graphics pipeline.
	ClearGraphicsPipeline _ClearGraphicsPipeline;

	//The particle system compute pipeline.
	ParticleSystemComputePipeline _ParticleSystemComputePipeline;

	//The masked model depth scene features graphics pipelines.
	StaticArray<MaskedModelDepthSceneFeaturesGraphicsPipeline, 2> _MaskedModelDepthSceneFeaturesGraphicsPipelines;

	//The particle system masked depth graphics pipeline.
	ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline _ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline;

	//The instanced static model depth scene features graphics pipelines.
	StaticArray<InstancedStaticModelDepthSceneFeaturesGraphicsPipeline, 2> _InstancedStaticModelDepthSceneFeaturesGraphicsPipelines;

	//The instanced impostor depth scene features graphics pipeline.
	InstancedImpostorDepthSceneFeaturesGraphicsPipeline _InstancedImpostorDepthSceneFeaturesGraphicsPipeline;

	//The masked model color scene features graphics pipelines.
	StaticArray<MaskedModelColorSceneFeaturesGraphicsPipeline, 2> _MaskedModelColorSceneFeaturesGraphicsPipelines;

	//The particle system masked color graphics pipeline.
	ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline _ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline;

	//The instanced impostor color scene features graphics pipeline.
	InstancedImpostorColorSceneFeaturesGraphicsPipeline _InstancedImpostorColorSceneFeaturesGraphicsPipeline;

	//The terrain scene features graphics pipeline.
	TerrainSceneFeaturesGraphicsPipeline _TerrainSceneFeaturesGraphicsPipeline;

	//The opaque model scene features graphics pipelines.
	StaticArray<OpaqueModelSceneFeaturesGraphicsPipeline, 2> _OpaqueModelSceneFeaturesGraphicsPipelines;

	//The instanced static model color scene features graphics pipelines.
	StaticArray<InstancedStaticModelColorSceneFeaturesGraphicsPipeline, 2> _InstancedStaticModelColorSceneFeaturesGraphicsPipelines;

	//The animated model scene features graphics pipeline.
	AnimatedModelSceneFeaturesGraphicsPipeline _AnimatedModelSceneFeaturesGraphicsPipeline;

	//The procedural grass graphics pipeline.
	ProceduralGrassSceneFeaturesGraphicsPipeline _ProceduralGrassSceneFeaturesGraphicsPipeline;

#if defined(CATALYST_EDITOR)
	//The editor selected model graphics pipeline.
	EditorSelectedModelGraphicsPipeline _EditorSelectedModelGraphicsPipeline;
#endif

	//The velocity graphics pipeline.
	VelocityGraphicsPipeline _VelocityGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif