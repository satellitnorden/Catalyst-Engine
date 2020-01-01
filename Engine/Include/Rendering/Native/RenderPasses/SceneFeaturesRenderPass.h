#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ParticleSystemComputePipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/AnimatedModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelHighlightSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationImpostorColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationImpostorDepthSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationSceneFeaturesGraphicsPipeline.h>
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

	//The particle system compute pipeline.
	ParticleSystemComputePipeline _ParticleSystemComputePipeline;

	//The particle system masked depth graphics pipeline.
	ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline _ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline;

	//The vegetation depth scene features graphics pipeline.
	VegetationDepthSceneFeaturesGraphicsPipeline _VegetationDepthSceneFeaturesGraphicsPipeline;

	//The vegetation impostor depth scene features graphics pipeline.
	VegetationImpostorDepthSceneFeaturesGraphicsPipeline _VegetationImpostorDepthSceneFeaturesGraphicsPipeline;

	//The terrain scene features graphics pipeline.
	TerrainSceneFeaturesGraphicsPipeline _TerrainSceneFeaturesGraphicsPipeline;

	//The particle system masked color graphics pipeline.
	ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline _ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline;

	//The vegetation opaque single sided scene features graphics pipeline.
	VegetationSceneFeaturesGraphicsPipeline _VegetationOpaqueSingleSidedSceneFeaturesGraphicsPipeline;

	//The vegetation opaque double sided scene features graphics pipeline.
	VegetationSceneFeaturesGraphicsPipeline _VegetationOpaqueDoubleSidedSceneFeaturesGraphicsPipeline;

	//The vegetation color scene features graphics pipeline.
	VegetationColorSceneFeaturesGraphicsPipeline _VegetationColorSceneFeaturesGraphicsPipeline;

	//The vegetation impostor color scene features graphics pipeline.
	VegetationImpostorColorSceneFeaturesGraphicsPipeline _VegetationImpostorColorSceneFeaturesGraphicsPipeline;

	//The model scene features graphics pipeline.
	ModelSceneFeaturesGraphicsPipeline _ModelSceneFeaturesGraphicsPipeline;

	//The animated model scene features graphics pipeline.
	AnimatedModelSceneFeaturesGraphicsPipeline _AnimatedModelSceneFeaturesGraphicsPipeline;

	//The model highlight scene features graphics pipeline.
	ModelHighlightSceneFeaturesGraphicsPipeline _ModelHighlightSceneFeaturesGraphicsPipeline;

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