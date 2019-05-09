//Header file.
#include <Rendering/Native/RenderPasses/TemporalAccumulationRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationRenderPass);

/*
*	Default constructor.
*/
TemporalAccumulationRenderPass::TemporalAccumulationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalAccumulation);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAccumulationGraphicsPipelines.Size());

	_TemporalAccumulationGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1), RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2));
	_TemporalAccumulationGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2), RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1));

	AddPipeline(&_TemporalAccumulationGraphicsPipelines[0]);
	AddPipeline(&_TemporalAccumulationGraphicsPipelines[1]);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void TemporalAccumulationRenderPass::Execute() NOEXCEPT
{
	//Execute the current graphics pipeline.
	_TemporalAccumulationGraphicsPipelines[_CurrentBufferIndex].Execute();

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == 0 ? 1 : 0;

	//Don't include the other graphics pipeline in the render.
	_TemporalAccumulationGraphicsPipelines[_CurrentBufferIndex].SetIncludeInRender(false);
}