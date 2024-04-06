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
	SetNumberOfPipelines(1 + 1 + _VolumetricLightingTemporalDenoisingPipelines.Size() + _VolumetricLightingSpatialDenoisingPipelines.Size() + 1);
	
	AddPipeline(&_VolumetricLightingGraphicsPipeline);
	AddPipeline(&_VolumetricLightingRayTracingPipeline);

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingTemporalDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
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

	for (uint64 i{ 0 }; i < _VolumetricLightingTemporalDenoisingPipelines.Size(); ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("VolumetricLightingNearest"), _VolumetricLightingRenderTarget);
		parameters._InputRenderTargets.Emplace(HashString("VolumetricLightingLinear"), _VolumetricLightingRenderTarget);
		parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), BaseMath::IsEven(i) ? _VolumetricLightingTemporalBufferRenderTargets[0] : _VolumetricLightingTemporalBufferRenderTargets[1]);

		parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), BaseMath::IsEven(i) ? _VolumetricLightingTemporalBufferRenderTargets[1] : _VolumetricLightingTemporalBufferRenderTargets[0]);
		parameters._OutputRenderTargets.Emplace(HashString("CurrentVolumetricLighting"), _IntermediateVolumetricLightingRenderTarget);

		_VolumetricLightingTemporalDenoisingPipelines[i].Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _VolumetricLightingSpatialDenoisingPipelines.Size(); ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputVolumetricLighting"), BaseMath::IsEven(i) ? _IntermediateVolumetricLightingRenderTarget : _VolumetricLightingRenderTarget);
		parameters._OutputRenderTargets.Emplace(HashString("OutputVolumetricLighting"), BaseMath::IsEven(i) ? _VolumetricLightingRenderTarget : _IntermediateVolumetricLightingRenderTarget);

		_VolumetricLightingSpatialDenoisingPipelines[i].Initialize(parameters);
	}

	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("VolumetricLighting"), _IntermediateVolumetricLightingRenderTarget);

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
		//Execute the current buffer, don't include the rest.
		for (uint64 i{ 0 }, size{ _VolumetricLightingTemporalDenoisingPipelines.Size() }; i < size; ++i)
		{
			if (i == _CurrentTemporalBufferIndex)
			{
				_VolumetricLightingTemporalDenoisingPipelines[i].Execute();
			}

			else
			{
				_VolumetricLightingTemporalDenoisingPipelines[i].SetIncludeInRender(false);
			}
		}

		for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
		{
			pipeline.Execute();
		}
	}
	
	else
	{
		for (GraphicsRenderPipeline &pipeline : _VolumetricLightingTemporalDenoisingPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	//Update the current temporal buffer index.
	_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _VolumetricLightingTemporalDenoisingPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

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

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingTemporalDenoisingPipelines)
	{
		pipeline.Execute();
	}

	for (GraphicsRenderPipeline &pipeline : _VolumetricLightingSpatialDenoisingPipelines)
	{
		pipeline.Terminate();
	}

	_VolumetricLightingApplicationPipeline.Terminate();
}