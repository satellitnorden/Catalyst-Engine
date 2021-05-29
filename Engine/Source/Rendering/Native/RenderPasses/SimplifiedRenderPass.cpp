#if defined(CATALYST_SIMPLIFIED_RENDERING)
//Header file.
#include <Rendering/Native/RenderPasses/SimplifiedRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Constants.
#define USE_SKY (0)

//Singleton definition.
DEFINE_SINGLETON(SimplifiedRenderPass);

/*
*	Default constructor.
*/
SimplifiedRenderPass::SimplifiedRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::SIMPLIFIED);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SimplifiedRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SimplifiedRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SimplifiedRenderPass::Initialize() NOEXCEPT
{
	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetFullResolution(), &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(4);
	AddPipeline(&_ClearGraphicsPipeline);
	AddPipeline(&_SimplifiedModelGraphicsPipeline);
#if USE_SKY
	AddPipeline(&_SimplifiedSkyGraphicsPipeline);
#endif
	AddPipeline(&_UserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_ClearGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_SimplifiedModelGraphicsPipeline.Initialize(_SceneDepthBuffer);
#if USE_SKY
	_SimplifiedSkyGraphicsPipeline.Initialize(_SceneDepthBuffer);
#endif
	_UserInterfaceGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SimplifiedRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_ClearGraphicsPipeline.Execute();
	_SimplifiedModelGraphicsPipeline.Execute();
#if USE_SKY
	_SimplifiedSkyGraphicsPipeline.Execute();
#endif
	_UserInterfaceGraphicsPipeline.Execute();

	//Enable this render pass.
	SetEnabled(true);
}
#endif