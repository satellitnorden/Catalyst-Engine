#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationColorSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationDepthSceneFeaturesGraphicsPipeline.h>
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

private:

	//The scene depth buffer.
	DepthBufferHandle _SceneDepthBuffer;

	//The vegetation depth scene features graphics pipeline.
	VegetationDepthSceneFeaturesGraphicsPipeline _VegetationDepthSceneFeaturesGraphicsPipeline;

	//The model scene features graphics pipeline.
	ModelSceneFeaturesGraphicsPipeline _ModelSceneFeaturesGraphicsPipeline;

	//The terrain scene features graphics pipeline.
	TerrainSceneFeaturesGraphicsPipeline _TerrainSceneFeaturesGraphicsPipeline;

	//The vegetation color scene features graphics pipeline.
	VegetationColorSceneFeaturesGraphicsPipeline _VegetationColorSceneFeaturesGraphicsPipeline;

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