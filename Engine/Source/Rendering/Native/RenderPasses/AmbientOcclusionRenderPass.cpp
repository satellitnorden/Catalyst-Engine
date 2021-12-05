//Header file.
#include <Rendering/Native/RenderPasses/AmbientOcclusionRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
AmbientOcclusionRenderPass::AmbientOcclusionRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::AMBIENT_OCCLUSION, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void AmbientOcclusionRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the ambient occlusion render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::R_UINT8, &_AmbientOcclusionRenderTarget);

	//Create the ambient occlusion temporal buffer render targets.
	for (RenderTargetHandle &render_target : _AmbientOcclusionTemporalBufferRenderTargets)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::R_UINT8, &render_target);
	}

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
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Initialize(_AmbientOcclusionRenderTarget);
	_AmbientOcclusionRayTracingPipeline.Initialize(_AmbientOcclusionRenderTarget);
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[0].Initialize(	_AmbientOcclusionRenderTarget,
																		1,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF));
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF),
																		2,
																		_AmbientOcclusionRenderTarget);
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[2].Initialize(	_AmbientOcclusionRenderTarget,
																		3,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF));
	_AmbientOcclusionSpatialDenoisingGraphicsPipelines[3].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_R_UINT8_HALF),
																		4,
																		_AmbientOcclusionRenderTarget);
	_AmbientOcclusionTemporalDenoisingGraphicsPipelines[0].Initialize(	_AmbientOcclusionTemporalBufferRenderTargets[0],
																		_AmbientOcclusionTemporalBufferRenderTargets[1],
																		_AmbientOcclusionRenderTarget);
	_AmbientOcclusionTemporalDenoisingGraphicsPipelines[1].Initialize(	_AmbientOcclusionTemporalBufferRenderTargets[1],
																		_AmbientOcclusionTemporalBufferRenderTargets[0],
																		_AmbientOcclusionRenderTarget);
	_AmbientOcclusionApplicationGraphicsPipeline.Initialize(_AmbientOcclusionRenderTarget);
}

/*
*	Executes this render pass.
*/
void AmbientOcclusionRenderPass::Execute() NOEXCEPT
{	
	//Nothing to do here if ambient occlusion isn't enabled.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::SCREEN_SPACE)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.Execute();
		_AmbientOcclusionRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.SetIncludeInRender(false);
		_AmbientOcclusionRayTracingPipeline.Execute();
	}
	
	if (!RenderingSystem::Instance->IsTakingScreenshot())
	{
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
	}
	
	else
	{
		for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		for (AmbientOcclusionTemporalDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	_AmbientOcclusionApplicationGraphicsPipeline.Execute();

	//Enable this render pass.
	SetEnabled(true);
}

/*
*	Terminates this render pass.
*/
void AmbientOcclusionRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Terminate();
	_AmbientOcclusionRayTracingPipeline.Terminate();

	for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (AmbientOcclusionTemporalDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_AmbientOcclusionApplicationGraphicsPipeline.Terminate();

	//Destroy the ambient occlusion render target.
	RenderingSystem::Instance->DestroyRenderTarget(&_AmbientOcclusionRenderTarget);

	//Destroy the ambient occlusion temporal buffer render targets.
	for (RenderTargetHandle &render_target : _AmbientOcclusionTemporalBufferRenderTargets)
	{
		RenderingSystem::Instance->DestroyRenderTarget(&render_target);
	}
}