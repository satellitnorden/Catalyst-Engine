//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingRenderPass);

/*
*	Default constructor.
*/
IndirectLightingRenderPass::IndirectLightingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::IndirectLighting);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IndirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IndirectLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void IndirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _IndirectLightingDenoisingGraphicsPipelines.Size() + _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() + 1);

	AddPipeline(&_ScreenSpaceIndirectLightingGraphicsPipeline);
	AddPipeline(&_IndirectLightingRayTracingPipeline);

	for (IndirectLightingDenoisingGraphicsPipeline &pipeline : _IndirectLightingDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_IndirectLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceIndirectLightingGraphicsPipeline.Initialize();
	_IndirectLightingRayTracingPipeline.Initialize();
	_IndirectLightingDenoisingGraphicsPipelines[0].Initialize(	IndirectLightingDenoisingGraphicsPipeline::Direction::Horizontal,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2));

	_IndirectLightingDenoisingGraphicsPipelines[1].Initialize(	IndirectLightingDenoisingGraphicsPipeline::Direction::Vertical,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalIndirectLightingBuffer2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalIndirectLightingBuffer1));

	_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalIndirectLightingBuffer1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalIndirectLightingBuffer2));

	_IndirectLightingApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	if (false)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetIndirectLightingMode() == RenderingConfigurationManager::IndirectLightingMode::NONE)
	{
		_ScreenSpaceIndirectLightingGraphicsPipeline.SetIncludeInRender(false);
		_IndirectLightingRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingConfigurationManager::Instance->GetIndirectLightingMode() == RenderingConfigurationManager::IndirectLightingMode::SCREEN_SPACE)
	{
		_ScreenSpaceIndirectLightingGraphicsPipeline.Execute();
		_IndirectLightingRayTracingPipeline.SetIncludeInRender(false);
	}

	if (RenderingConfigurationManager::Instance->GetIndirectLightingMode() != RenderingConfigurationManager::IndirectLightingMode::NONE)
	{
		for (IndirectLightingDenoisingGraphicsPipeline& pipeline : _IndirectLightingDenoisingGraphicsPipelines)
		{
			pipeline.Execute();
		}
	}

	else
	{
		for (IndirectLightingDenoisingGraphicsPipeline& pipeline : _IndirectLightingDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	//Execute the current buffer, don't include the rest.
	if (RenderingConfigurationManager::Instance->GetIndirectLightingMode() != RenderingConfigurationManager::IndirectLightingMode::NONE)
	{
		for (uint64 i{ 0 }, size{ _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() }; i < size; ++i)
		{
			if (i == _CurrentTemporalBufferIndex)
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[i].Execute();
			}

			else
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
			}
		}
	}

	else
	{
		for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

	_IndirectLightingApplicationGraphicsPipeline.Execute();
}