//Header file.
#include <Rendering/Native/RenderPasses/PathTracingRayTracingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PathTracingRayTracingRenderPass);

/*
*	Default constructor.
*/
PathTracingRayTracingRenderPass::PathTracingRayTracingRenderPass() NOEXCEPT
{
	//Set the stage.
#if defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::PathTracing);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PathTracingRayTracingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PathTracingRayTracingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void PathTracingRayTracingRenderPass::Initialize() NOEXCEPT
{
	//Define constants.
	constexpr float PATH_TRACING_FEEDBACK_FACTOR{ 0.9f }; //0.0025f step.

	//Create the temporal accumulation render targets.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_TemporalAccumulationRenderTargets[0]);
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_TemporalAccumulationRenderTargets[1]);

	//Add the pipelines.
	SetNumberOfPipelines(1 + _PathTracingDenoisingGraphicsPipelines.Size() + _TemporalAccumulationGraphicsPipeline.Size());

	//Initialize all pipelines.
	_PathTracingRayTracingPipeline.Initialize();
	_PathTracingDenoisingGraphicsPipelines[0].Initialize(	PathTracingDenoisingGraphicsPipeline::Direction::Horizontal,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float));
	_PathTracingDenoisingGraphicsPipelines[1].Initialize(	PathTracingDenoisingGraphicsPipeline::Direction::Vertical,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));
	_TemporalAccumulationGraphicsPipeline[0].Initialize(_TemporalAccumulationRenderTargets[1],
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														_TemporalAccumulationRenderTargets[0],
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														PATH_TRACING_FEEDBACK_FACTOR,
														RenderingSystem::Instance->GetScaledResolution());
	_TemporalAccumulationGraphicsPipeline[1].Initialize(_TemporalAccumulationRenderTargets[0],
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														_TemporalAccumulationRenderTargets[1],
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														PATH_TRACING_FEEDBACK_FACTOR,
														RenderingSystem::Instance->GetScaledResolution());

	AddPipeline(&_PathTracingRayTracingPipeline);
	AddPipeline(&_PathTracingDenoisingGraphicsPipelines[0]);
	AddPipeline(&_PathTracingDenoisingGraphicsPipelines[1]);
	AddPipeline(&_TemporalAccumulationGraphicsPipeline[0]);
	AddPipeline(&_TemporalAccumulationGraphicsPipeline[1]);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void PathTracingRayTracingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_PathTracingRayTracingPipeline.Execute();

	/*
	if (false)
	{
		_PathTracingDenoisingGraphicsPipelines[0].Execute();
		_PathTracingDenoisingGraphicsPipelines[1].Execute();
	}
	
	else
	{
		_PathTracingDenoisingGraphicsPipelines[0].SetIncludeInRender(false);
		_PathTracingDenoisingGraphicsPipelines[1].SetIncludeInRender(false);
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _TemporalAccumulationGraphicsPipeline.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex)
		{
			_TemporalAccumulationGraphicsPipeline[i].Execute();
		}

		else
		{
			_TemporalAccumulationGraphicsPipeline[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _TemporalAccumulationGraphicsPipeline.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
	*/
}