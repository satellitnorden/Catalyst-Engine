//Header file.
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostSceneFeaturesRenderPass);

/*
*	Default constructor.
*/
PostSceneFeaturesRenderPass::PostSceneFeaturesRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::POST_SCENE_FEATURES, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void PostSceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_SceneFeaturesDownsampleGraphicsPipeline);

	//Initialize all pipelines.
	_SceneFeaturesDownsampleGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void PostSceneFeaturesRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_SceneFeaturesDownsampleGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void PostSceneFeaturesRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_SceneFeaturesDownsampleGraphicsPipeline.Terminate();
}