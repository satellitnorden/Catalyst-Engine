#if !defined(CATALYST_SIMPLIFIED_RENDERING)
//Header file.
#include <Rendering/Native/RenderPasses/VolumetricLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VolumetricLightingRenderPass);

/*
*	Default constructor.
*/
VolumetricLightingRenderPass::VolumetricLightingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::VOLUMETRIC_LIGHTING);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		VolumetricLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		VolumetricLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void VolumetricLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _VolumetricLightingSpatialDenoisingGraphicsPipelines.Size() + _VolumetricLightingTemporalDenoisingGraphicsPipelines.Size() + 2);
	
	AddPipeline(&_VolumetricLightingGraphicsPipeline);
	AddPipeline(&_VolumetricLightingRayTracingPipeline);

	for (VolumetricLightingSpatialDenoisingGraphicsPipeline &pipeline : _VolumetricLightingSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (VolumetricLightingTemporalDenoisingGraphicsPipeline &pipeline : _VolumetricLightingTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_VolumetricLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_VolumetricLightingGraphicsPipeline.Initialize();
	_VolumetricLightingRayTracingPipeline.Initialize();
	_VolumetricLightingSpatialDenoisingGraphicsPipelines[0].Initialize(	1,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_VolumetricLightingSpatialDenoisingGraphicsPipelines[1].Initialize(	2,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
	_VolumetricLightingTemporalDenoisingGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_1));
	_VolumetricLightingTemporalDenoisingGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_2));
	_VolumetricLightingApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void VolumetricLightingRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this rendering path.
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::DEFAULT
		|| !WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetVolumetricShadowsMode() == RenderingConfiguration::VolumetricShadowsMode::NONE
		|| RenderingSystem::Instance->GetRenderingConfiguration()->GetVolumetricShadowsMode() == RenderingConfiguration::VolumetricShadowsMode::SCREEN_SPACE)
	{
		_VolumetricLightingGraphicsPipeline.Execute();
		_VolumetricLightingRayTracingPipeline.SetIncludeInRender(false);
	}

	else
	{
		_VolumetricLightingGraphicsPipeline.SetIncludeInRender(false);
		_VolumetricLightingRayTracingPipeline.Execute();
	}

	if (!RenderingSystem::Instance->IsTakingScreenshot())
	{
		for (VolumetricLightingSpatialDenoisingGraphicsPipeline &pipeline : _VolumetricLightingSpatialDenoisingGraphicsPipelines)
		{
			pipeline.Execute();
		}

		//Execute the current buffer, don't include the rest.
		for (uint64 i{ 0 }, size{ _VolumetricLightingTemporalDenoisingGraphicsPipelines.Size() }; i < size; ++i)
		{
			if (i == _CurrentTemporalBufferIndex)
			{
				_VolumetricLightingTemporalDenoisingGraphicsPipelines[i].Execute();
			}

			else
			{
				_VolumetricLightingTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
			}
		}
	}
	
	else
	{
		for (VolumetricLightingSpatialDenoisingGraphicsPipeline &pipeline : _VolumetricLightingSpatialDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		for (VolumetricLightingTemporalDenoisingGraphicsPipeline &pipeline : _VolumetricLightingTemporalDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	//Update the current temporal buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _VolumetricLightingTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

	_VolumetricLightingApplicationGraphicsPipeline.Execute();
}
#endif