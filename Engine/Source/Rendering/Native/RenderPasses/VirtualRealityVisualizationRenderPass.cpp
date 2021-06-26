#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Rendering/Native/RenderPasses/VirtualRealityVisualizationRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VirtualRealityVisualizationRenderPass);

/*
*	Default constructor.
*/
VirtualRealityVisualizationRenderPass::VirtualRealityVisualizationRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterVirtualRealityRenderPass(VirtualRealityNativeRenderPassStage::VIRTUAL_REALITY_VISUALIZATION, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		VirtualRealityVisualizationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		VirtualRealityVisualizationRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		VirtualRealityVisualizationRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void VirtualRealityVisualizationRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();
}

/*
*	Executes this render pass.
*/
void VirtualRealityVisualizationRenderPass::Execute() NOEXCEPT
{
	//Completely disabled, for now.
	SetEnabled(false);
}

/*
*	Terminates this render pass.
*/
void VirtualRealityVisualizationRenderPass::Terminate() NOEXCEPT
{
	
}
#endif