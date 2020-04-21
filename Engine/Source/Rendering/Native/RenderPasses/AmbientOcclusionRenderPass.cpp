//Header file.
#include <Rendering/Native/RenderPasses/AmbientOcclusionRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
AmbientOcclusionRenderPass::AmbientOcclusionRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::AMBIENT_OCCLUSION);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void AmbientOcclusionRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _AmbientOcclusionSpatialDenoisingGraphicsPipelines.Size() + _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() + 1);
	AddPipeline(&_ScreenSpaceAmbientOcclusionGraphicsPipeline);
	AddPipeline(&_AmbientOcclusionRayTracingPipeline);

	for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (AmbientOcclusionTemporalDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_AmbientOcclusionApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Initialize();
	_AmbientOcclusionRayTracingPipeline.Initialize();
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[0].Initialize(	1,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF));
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[1].Initialize(	2,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION));
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[2].Initialize(	3,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF));
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[3].Initialize(	4,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION));
	_AmbientOcclusionTemporalDenoisingGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_1));
	_AmbientOcclusionTemporalDenoisingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_2));
	_AmbientOcclusionApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void AmbientOcclusionRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	//Nothing to do here if ambient occlusion isn't enabled.
	if (RenderingConfigurationManager::Instance->GetAmbientOcclusionMode() == RenderingConfigurationManager::AmbientOcclusionMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetAmbientOcclusionMode() == RenderingConfigurationManager::AmbientOcclusionMode::SCREEN_SPACE)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.Execute();
		_AmbientOcclusionRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingConfigurationManager::Instance->GetAmbientOcclusionMode() == RenderingConfigurationManager::AmbientOcclusionMode::RAY_TRACED)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.SetIncludeInRender(false);
		_AmbientOcclusionRayTracingPipeline.Execute();
	}
	
	for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentTemporalBufferIndex)
		{
			_AmbientOcclusionTemporalDenoisingGraphicsPipelines[i].Execute();
		}

		else
		{
			_AmbientOcclusionTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

	_AmbientOcclusionApplicationGraphicsPipeline.Execute();
}