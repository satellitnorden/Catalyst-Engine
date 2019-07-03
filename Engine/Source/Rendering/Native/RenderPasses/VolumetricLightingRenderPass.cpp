//Header file.
#include <Rendering/Native/RenderPasses/VolumetricLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VolumetricLightingRenderPass);

/*
*	Default constructor.
*/
VolumetricLightingRenderPass::VolumetricLightingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::VolumetricLighting);
#endif

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
	SetNumberOfPipelines(1 + _VolumetricLightingDenoisingGraphicsPipelines.Size() + 2);
	AddPipeline(&_VolumetricLightingRayTracingPipeline);

	for (VolumetricLightingDenoisingGraphicsPipeline &pipeline : _VolumetricLightingDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_VolumetricLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_VolumetricLightingRayTracingPipeline.Initialize();
	_VolumetricLightingDenoisingGraphicsPipelines[0].Initialize(VolumetricLightingDenoisingGraphicsPipeline::Direction::Horizontal,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_2));

	_VolumetricLightingDenoisingGraphicsPipelines[1].Initialize(VolumetricLightingDenoisingGraphicsPipeline::Direction::Vertical,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_2),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));

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
	//Execute all pipelines.
	_VolumetricLightingRayTracingPipeline.Execute();

	for (VolumetricLightingDenoisingGraphicsPipeline &pipeline : _VolumetricLightingDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_VolumetricLightingApplicationGraphicsPipeline.Execute();
}