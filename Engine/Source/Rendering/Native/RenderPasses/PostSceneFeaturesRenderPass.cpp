//Header file.
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

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
	SetNumberOfPipelines(2);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	AddPipeline(&_OnlyLightingPipeline);
#endif
	AddPipeline(&_SceneFeaturesDownsampleGraphicsPipeline);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	_OnlyLightingPipeline.Initialize();
#endif
	_SceneFeaturesDownsampleGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void PostSceneFeaturesRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_OnlyLightingPipeline.Execute();
#endif
	_SceneFeaturesDownsampleGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void PostSceneFeaturesRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_OnlyLightingPipeline.Terminate();
#endif
	_SceneFeaturesDownsampleGraphicsPipeline.Terminate();
}