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
	SetStage(RenderPassStage::INDIRECT_LIGHTING);

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
	SetNumberOfPipelines(1 + 1 + _IndirectLightingSpatialDenoisingGraphicsPipelines.Size() + _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() + 1);

	AddPipeline(&_ScreenSpaceIndirectLightingGraphicsPipeline);
	AddPipeline(&_IndirectLightingRayTracingPipeline);

	for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
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
	_IndirectLightingSpatialDenoisingGraphicsPipelines[0].Initialize(	1,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_IndirectLightingSpatialDenoisingGraphicsPipelines[1].Initialize(	2,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_1));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_2));

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
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
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

	if (RenderingConfigurationManager::Instance->GetIndirectLightingMode() != RenderingConfigurationManager::IndirectLightingMode::NONE && false)
	{
		for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
		{
			pipeline.Execute();
		}
	}

	else
	{
		for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
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

	_IndirectLightingApplicationGraphicsPipeline.Execute();

	//Update the current buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;
}