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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		DirectLightingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingRenderPass::Initialize() NOEXCEPT
{	
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DirectLightingGraphicsPipeline);

	//Initialize all pipelines.
	_DirectLightingGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void DirectLightingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_DirectLightingGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void DirectLightingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_DirectLightingGraphicsPipeline.Terminate();
}