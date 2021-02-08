//Header file.
#include <Rendering/Native/RenderPasses/DepthOfFieldRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DepthOfFieldRenderPass);

/*
*	Default constructor.
*/
DepthOfFieldRenderPass::DepthOfFieldRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::DEPTH_OF_FIELD);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DepthOfFieldRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(2);

	AddPipeline(&_DepthOfFieldBokehBlurGraphicsPipeline);

	AddPipeline(&_DepthOfFieldApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Initialize();
	_DepthOfFieldApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DepthOfFieldRenderPass::Execute() NOEXCEPT
{
	//No need to execute if depth of field size is zero.
	if (RenderingSystem::Instance->GetPostProcessingSystem()->GetDepthOfFieldSize() == 0.0f)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Execute();
	_DepthOfFieldApplicationGraphicsPipeline.Execute();
}