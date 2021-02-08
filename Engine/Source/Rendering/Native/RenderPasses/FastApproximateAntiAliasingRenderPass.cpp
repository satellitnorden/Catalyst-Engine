//Header file.
#include <Rendering/Native/RenderPasses/FastApproximateAntiAliasingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(FastApproximateAntiAliasingRenderPass);

/*
*	Default constructor.
*/
FastApproximateAntiAliasingRenderPass::FastApproximateAntiAliasingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::FAST_APPROXIMATE_ANTI_ALIASING);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		FastApproximateAntiAliasingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		FastApproximateAntiAliasingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void FastApproximateAntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_FastApproximateAntiAliasingGraphicsPipeline);

	//Initialize all pipelines.
	_FastApproximateAntiAliasingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void FastApproximateAntiAliasingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_FastApproximateAntiAliasingGraphicsPipeline.Execute();
}