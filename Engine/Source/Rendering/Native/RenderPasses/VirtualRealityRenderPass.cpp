//Header file.
#include <Rendering/Native/RenderPasses/VirtualRealityRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VirtualRealityRenderPass);

/*
*	Default constructor.
*/
VirtualRealityRenderPass::VirtualRealityRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterVirtualRealityRenderPass(VirtualRealityNativeRenderPassStage::VIRTUAL_REALITY, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		VirtualRealityRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		VirtualRealityRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		VirtualRealityRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void VirtualRealityRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add all pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_VirtualRealityGraphicsPipeline);

	//Initialize all pipelines.
	_VirtualRealityGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void VirtualRealityRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_VirtualRealityGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void VirtualRealityRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_VirtualRealityGraphicsPipeline.Terminate();
}