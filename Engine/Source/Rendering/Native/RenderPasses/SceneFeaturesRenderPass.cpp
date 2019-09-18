//Header file.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SceneFeaturesRenderPass);

/*
*	Default constructor.
*/
SceneFeaturesRenderPass::SceneFeaturesRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::SceneFeatures);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(), &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(9);
	AddPipeline(&_ModelSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationDepthSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationImpostorDepthSceneFeaturesGraphicsPipeline);
	AddPipeline(&_TerrainSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationImpostorColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_AnimatedModelSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VelocityGraphicsPipeline);

	//Initialize all pipelines.
	_VegetationDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationImpostorDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_TerrainSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationImpostorColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_ModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VelocityGraphicsPipeline.Initialize(_SceneDepthBuffer);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SceneFeaturesRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_VegetationDepthSceneFeaturesGraphicsPipeline.Execute();
	_VegetationImpostorDepthSceneFeaturesGraphicsPipeline.Execute();
	_TerrainSceneFeaturesGraphicsPipeline.Execute();
	_VegetationSceneFeaturesGraphicsPipeline.Execute();
	_VegetationColorSceneFeaturesGraphicsPipeline.Execute();
	_VegetationImpostorColorSceneFeaturesGraphicsPipeline.Execute();
	_ModelSceneFeaturesGraphicsPipeline.Execute();
	_AnimatedModelSceneFeaturesGraphicsPipeline.Execute();
	_VelocityGraphicsPipeline.Execute();
}