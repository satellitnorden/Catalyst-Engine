//Header file.
#include <Rendering/Native/RenderPasses/MobileRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
MobileRenderPass::MobileRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterMobileRenderPass(MobileNativeRenderPassStage::MOBILE, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		MobileRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		MobileRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		MobileRenderPass::Instance->Terminate();
	});

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Mobile");
#endif
}

/*
*	Initializes this render pass.
*/
void MobileRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::D_UINT24_S_UINT8, SampleCount::SAMPLE_COUNT_1, &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_MobileGraphicsPipeline);

	//Initialize all pipelines.
	_MobileGraphicsPipeline.Initialize(_SceneDepthBuffer);
}

/*
*	Executes this render pass.
*/
void MobileRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_MobileGraphicsPipeline.Execute();

	//Enable this render pass.
	SetEnabled(true);
}

/*
*	Terminates this render pass.
*/
void MobileRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_MobileGraphicsPipeline.Terminate();
}