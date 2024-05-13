//Header file.
#include <Rendering/Native/RenderPasses/ToneMappingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ToneMappingRenderPass::ToneMappingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::TONE_MAPPING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ToneMappingRenderPass::Instance->Initialize();
	});

	//Set the pre-record function.
	SetPreRecordFunction([](CommandBuffer* const RESTRICT command_buffer)
	{
		ToneMappingRenderPass::Instance->PreRecord(command_buffer);
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ToneMappingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ToneMappingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ToneMappingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ToneMappingPipeline);

	//Initialize all pipelines.
	_ToneMappingPipeline.Initialize();
}

/*
*	Pre-record.
*/
void ToneMappingRenderPass::PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Blit the scene.
	command_buffer->BlitImage
	(
		nullptr,
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE),
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::PREVIOUS_SCENE)
	);
}

/*
*	Executes this render pass.
*/
void ToneMappingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ToneMappingPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void ToneMappingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ToneMappingPipeline.Terminate();
}