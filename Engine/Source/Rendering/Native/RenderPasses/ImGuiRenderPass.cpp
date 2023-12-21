#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/ImGuiRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ImGuiRenderPass);

/*
*	Default constructor.
*/
ImGuiRenderPass::ImGuiRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::IMGUI, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::IMGUI, this);
	NativeRenderPassManager::RegisterMobileRenderPass(MobileNativeRenderPassStage::IMGUI, this);
	NativeRenderPassManager::RegisterVirtualRealityRenderPass(VirtualRealityNativeRenderPassStage::IMGUI, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ImGuiRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ImGuiRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ImGuiRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ImGuiRenderPass::Initialize() NOEXCEPT
{	
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ImGuiGraphicsPipeline);

	//Initialize all pipelines.
	_ImGuiGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void ImGuiRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ImGuiGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void ImGuiRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ImGuiGraphicsPipeline.Terminate();
}
#endif