//Header file.
#include <Rendering/Native/RenderPasses/VolumetricLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

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
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::VOLUMETRIC_LIGHTING, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		VolumetricLightingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void VolumetricLightingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the volumetric lighting render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_VolumetricLightingRenderTarget);

	//Create the intermediate volumetric lighting render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_IntermediateVolumetricLightingRenderTarget);

	//Create the volumetric lighting temporal buffer render targets.
	for (RenderTargetHandle &render_target : _VolumetricLightingTemporalBufferRenderTargets)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &render_target);
	}

	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _VolumetricLightingSpatialDenoisingPipelines.Size() + _VolumetricLightingTemporalDenoisingGraphicsPipelines.Size() + 2);
	
	AddPipeline(&_VolumetricLightingGraphicsPipeline);
	AddPipeline(&_VolumetricLightingRayTracingPipeline);

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (VolumetricLightingTemporalDenoisingGraphicsPipeline &pipeline : _VolumetricLightingTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_VolumetricLightingApplicationPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._OutputRenderTargets.Emplace(HashString("VolumetricLighting"), _VolumetricLightingRenderTarget);

		_VolumetricLightingGraphicsPipeline.Initialize(parameters);
	}

	{
		RayTracingRenderPipelineParameters parameters;

		parameters._ComputeRenderTargets.Emplace(HashString("VolumetricLighting"), _VolumetricLightingRenderTarget);

		_VolumetricLightingRayTracingPipeline.Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _VolumetricLightingSpatialDenoisingPipelines.Size(); ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputVolumetricLighting"), CatalystBaseMath::IsEven(i) ? _VolumetricLightingRenderTarget : _IntermediateVolumetricLightingRenderTarget);
		parameters._OutputRenderTargets.Emplace(HashString("OutputVolumetricLighting"), CatalystBaseMath::IsEven(i) ? _IntermediateVolumetricLightingRenderTarget : _VolumetricLightingRenderTarget);

		_VolumetricLightingSpatialDenoisingPipelines[i].Initialize(parameters);
	}

	_VolumetricLightingTemporalDenoisingGraphicsPipelines[0].Initialize(_VolumetricLightingTemporalBufferRenderTargets[0],
																		_VolumetricLightingTemporalBufferRenderTargets[1],
																		_VolumetricLightingRenderTarget,
																		_IntermediateVolumetricLightingRenderTarget);
	_VolumetricLightingTemporalDenoisingGraphicsPipelines[1].Initialize(_VolumetricLightingTemporalBufferRenderTargets[1],
																		_VolumetricLightingTemporalBufferRenderTargets[0],
																		_VolumetricLightingRenderTarget,
																		_IntermediateVolumetricLightingRenderTarget);
	{
		ComputeRenderPipelineParameters parameters;

		parameters._ComputeRenderTargets.Emplace(HashString("VolumetricLighting"), _VolumetricLightingRenderTarget);

		_VolumetricLightingApplicationPipeline.Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void VolumetricLightingRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this render pass.
	if (!WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled)
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
		for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
		{
			pipeline.Execute();
		}
	}
	
	else
	{
		for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	for (VolumetricLightingTemporalDenoisingGraphicsPipeline& pipeline : _VolumetricLightingTemporalDenoisingGraphicsPipelines)
	{
		pipeline.SetIncludeInRender(false);
	}

	//Update the current temporal buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _VolumetricLightingTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

	_VolumetricLightingApplicationPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void VolumetricLightingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_VolumetricLightingGraphicsPipeline.Terminate();
	_VolumetricLightingRayTracingPipeline.Terminate();

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
	{
		pipeline.Terminate();
	}

	for (VolumetricLightingTemporalDenoisingGraphicsPipeline &pipeline : _VolumetricLightingTemporalDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_VolumetricLightingApplicationPipeline.Terminate();
}