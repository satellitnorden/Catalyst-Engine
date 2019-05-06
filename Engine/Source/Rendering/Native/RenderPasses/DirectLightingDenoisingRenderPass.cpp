//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingDenoisingRenderPass.h>

//Systems.
#include <Systems/LightingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingDenoisingRenderPass);

/*
*	Default constructor.
*/
DirectLightingDenoisingRenderPass::DirectLightingDenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectLightingDenoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingDenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingDenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingDenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(_VisibilityDenoisingGraphicsPipelines.Size());

	//Initialize all pipelines.
	_VisibilityDenoisingGraphicsPipelines[0].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Horizontal,
														LightingSystem::Instance->GetDirectionalLightVisibilityRenderTarget(),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));
	AddPipeline(&_VisibilityDenoisingGraphicsPipelines[0]);

	_VisibilityDenoisingGraphicsPipelines[1].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Vertical,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
														LightingSystem::Instance->GetDirectionalLightVisibilityRenderTarget());
	AddPipeline(&_VisibilityDenoisingGraphicsPipelines[1]);

	uint8 renderTargetCounter{ 0 };

	for (uint8 i{ 0 }; i < LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS * 2; i += 2)
	{
		_VisibilityDenoisingGraphicsPipelines[2 + i].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Horizontal,
																LightingSystem::Instance->GetLightVisibilityRenderTargets()[renderTargetCounter],
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));
		AddPipeline(&_VisibilityDenoisingGraphicsPipelines[2 + i]);

		_VisibilityDenoisingGraphicsPipelines[2 + i + 1].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Vertical,
																	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
																	LightingSystem::Instance->GetLightVisibilityRenderTargets()[renderTargetCounter]);
		AddPipeline(&_VisibilityDenoisingGraphicsPipelines[i + 1 + 2]);

		++renderTargetCounter;
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
void DirectLightingDenoisingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (VisibilityDenoisingGraphicsPipeline &pipeline : _VisibilityDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}
}