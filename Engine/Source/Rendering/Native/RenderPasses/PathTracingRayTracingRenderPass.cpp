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
	_TemporalAccumulationGraphicsPipeline[0].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer2),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														0.99f);
	_TemporalAccumulationGraphicsPipeline[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradianceTemporalAccumulationBuffer2),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
														0.99f);

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
	_PathTracingDenoisingGraphicsPipelines[0].Execute();
	_PathTracingDenoisingGraphicsPipelines[1].Execute();

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