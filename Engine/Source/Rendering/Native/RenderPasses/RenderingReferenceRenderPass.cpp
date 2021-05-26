#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingReferenceRenderPass);

/*
*	Default constructor.
*/
RenderingReferenceRenderPass::RenderingReferenceRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::RENDERING_REFERENCE);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		RenderingReferenceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		RenderingReferenceRenderPass::Instance->Execute();
	});
}

/*
*	Sets the properties.
*/
void RenderingReferenceRenderPass::SetProperties(const uint32 rendering_reference_texture_index, const uint32 iterations) NOEXCEPT
{
	_RenderingReferenceGraphicsPipeline.SetProperties(rendering_reference_texture_index, iterations);
}

/*
*	Initializes this render pass.
*/
void RenderingReferenceRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_RenderingReferenceGraphicsPipeline);

	//Initialize all pipelines.
	_RenderingReferenceGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void RenderingReferenceRenderPass::Execute() NOEXCEPT
{	
	//No need to execute if rendering reference is not in progress.
	if (RenderingSystem::Instance->GetRenderingReferenceSystem()->IsRenderingReferenceInProgress())
	{
		SetEnabled(true);

		_RenderingReferenceGraphicsPipeline.Execute();
	}

	else
	{
		SetEnabled(false);
	}
}
#endif
#endif