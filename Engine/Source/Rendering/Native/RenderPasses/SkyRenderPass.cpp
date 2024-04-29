//Header file.
#include <Rendering/Native/RenderPasses/SkyRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
SkyRenderPass::SkyRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SKY, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SkyRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SkyRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		SkyRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void SkyRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_SkyPipeline);

	//Initialize all pipelines.
	_SkyPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void SkyRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_SkyPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void SkyRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_SkyPipeline.Terminate();
}