//Header file.
#include <Rendering/Native/RenderPasses/WaterRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WaterRenderPass);

/*
*	Default constructor.
*/
WaterRenderPass::WaterRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::WATER, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		WaterRenderPass::Instance->Initialize();
	});

	//Set the pre-record function.
	SetPreRecordFunction([](CommandBuffer *const RESTRICT command_buffer)
	{
		WaterRenderPass::Instance->PreRecord(command_buffer);
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		WaterRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		WaterRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void WaterRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene features 1 render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &_SceneFeatures1RenderTarget);

	//Create the scene features 2 render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_SceneFeatures2RenderTarget);

	//Add the pipelines.
	SetNumberOfPipelines(1);

	AddPipeline(&_WaterRenderPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("SceneFeatures1Input"), _SceneFeatures1RenderTarget);
		parameters._InputRenderTargets.Emplace(HashString("SceneFeatures2Input"), _SceneFeatures2RenderTarget);

		_WaterRenderPipeline.Initialize(parameters);
	}
}

/*
*	Pre-record.
*/
void WaterRenderPass::PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Blit the scene features 1 render target.
	command_buffer->BlitImage
	(
		nullptr,
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1),
		_SceneFeatures1RenderTarget
	);

	//Blit the scene features 2 render target.
	command_buffer->BlitImage
	(
		nullptr,
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2),
		_SceneFeatures2RenderTarget
	);
}

/*
*	Executes this render pass.
*/
void WaterRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_WaterRenderPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void WaterRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_WaterRenderPipeline.Terminate();
}