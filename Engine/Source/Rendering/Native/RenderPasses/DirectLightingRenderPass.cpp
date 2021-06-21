//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingRenderPass);

/*
*	Default constructor.
*/
DirectLightingRenderPass::DirectLightingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::DIRECT_LIGHTING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingRenderPass::Initialize() NOEXCEPT
{	
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DirectLightingGraphicsPipeline);

	//Initialize all pipelines.
	_DirectLightingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DirectLightingRenderPass::Execute() NOEXCEPT
{
	//Selectively enable this rendering path.
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::DEFAULT)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	_DirectLightingGraphicsPipeline.Execute();
}