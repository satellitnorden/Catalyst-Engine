//Header file.
#include <Rendering/Native/RenderPasses/TemporalAccumulationRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationRenderPass);

/*
*	Default constructor.
*/
TemporalAccumulationRenderPass::TemporalAccumulationRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::TemporalAccumulation);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Define constants.
	constexpr float DIFFUSE_IRRADIANCE_FEEDBACK_FACTOR{ 0.9f }; //0.025f step.

	//Initialize and add the pipelines.
	SetNumberOfPipelines(_DiffuseIrradianceTemporalAccumulationGraphicsPipelines.Size());

	_DiffuseIrradianceTemporalAccumulationGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer2),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer1),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																			DIFFUSE_IRRADIANCE_FEEDBACK_FACTOR,
																			RenderingSystem::Instance->GetScaledResolution() / 2);

	_DiffuseIrradianceTemporalAccumulationGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer1),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer2),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																			DIFFUSE_IRRADIANCE_FEEDBACK_FACTOR,
																			RenderingSystem::Instance->GetScaledResolution() / 2);

	for (TemporalAccumulationGraphicsPipeline &pipeline : _DiffuseIrradianceTemporalAccumulationGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void TemporalAccumulationRenderPass::Execute() NOEXCEPT
{
	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _DiffuseIrradianceTemporalAccumulationGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex && RenderingConfigurationManager::Instance->GetDiffuseIrradianceMode() == RenderingConfigurationManager::DiffuseIrradianceMode::RayTraced)
		{
			_DiffuseIrradianceTemporalAccumulationGraphicsPipelines[i].Execute();
		}

		else
		{
			_DiffuseIrradianceTemporalAccumulationGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _DiffuseIrradianceTemporalAccumulationGraphicsPipelines.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
}