//Header file.
#include <Rendering/Native/RenderPasses/DepthOfFieldRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DepthOfFieldRenderPass);

/*
*	Default constructor.
*/
DepthOfFieldRenderPass::DepthOfFieldRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::DEPTH_OF_FIELD, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::DEPTH_OF_FIELD, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void DepthOfFieldRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1 + _DepthOfFieldFloodfillBlurGraphicsPipelines.Size() + 1);

	AddPipeline(&_DepthOfFieldBokehBlurGraphicsPipeline);

	for (DepthOfFieldFloodfillBlurGraphicsPipeline &pipeline : _DepthOfFieldFloodfillBlurGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_DepthOfFieldApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Initialize();

	_DepthOfFieldFloodfillBlurGraphicsPipelines[0].Initialize
	(
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2),
		0
	);
	_DepthOfFieldFloodfillBlurGraphicsPipelines[1].Initialize
	(
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2),
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
		1
	);

	_DepthOfFieldApplicationGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void DepthOfFieldRenderPass::Execute() NOEXCEPT
{
	//No need to execute if depth of field size is zero.
	if (RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetAperture() >= 1.0f)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Execute();
	_DepthOfFieldApplicationGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void DepthOfFieldRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Terminate();
	_DepthOfFieldApplicationGraphicsPipeline.Terminate();
}