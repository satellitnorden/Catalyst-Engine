//Header file.
#include <Rendering/Native/RenderPasses/VolumetricLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VolumetricLightingRenderPass);

//Volumetric lighting render pass constants.
namespace VolumetricLightingRenderPassConstants
{
	constexpr float VOLUMETRIC_LIGHTING_SEPARABLE_BLUR_SIZE{ 14.0f };
}

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
	SetNumberOfPipelines(1 + _VolumetricLightingSeparableGraphicsPipelines.Size() + 2);
	AddPipeline(&_VolumetricLightingRayTracingPipeline);

	for (SeparableBlurGraphicsPipeline &pipeline : _VolumetricLightingSeparableGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_VolumetricLightingResampleGraphicsPipeline);
	AddPipeline(&_VolumetricLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_VolumetricLightingRayTracingPipeline.Initialize();
	_VolumetricLightingSeparableGraphicsPipelines[0].Initialize(SeparableBlurGraphicsPipeline::Direction::Horizontal,
																VolumetricLightingRenderPassConstants::VOLUMETRIC_LIGHTING_SEPARABLE_BLUR_SIZE,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetScaledResolution() / 4);

	_VolumetricLightingSeparableGraphicsPipelines[1].Initialize(SeparableBlurGraphicsPipeline::Direction::Vertical,
																VolumetricLightingRenderPassConstants::VOLUMETRIC_LIGHTING_SEPARABLE_BLUR_SIZE,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
																RenderingSystem::Instance->GetScaledResolution() / 4);
	_VolumetricLightingSeparableGraphicsPipelines[2].Initialize(SeparableBlurGraphicsPipeline::Direction::Horizontal,
		VolumetricLightingRenderPassConstants::VOLUMETRIC_LIGHTING_SEPARABLE_BLUR_SIZE,
		2.0f,
		RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
		RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
		RenderingSystem::Instance->GetScaledResolution() / 4);

	_VolumetricLightingSeparableGraphicsPipelines[3].Initialize(SeparableBlurGraphicsPipeline::Direction::Vertical,
		VolumetricLightingRenderPassConstants::VOLUMETRIC_LIGHTING_SEPARABLE_BLUR_SIZE,
		2.0f,
		RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
		RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
		RenderingSystem::Instance->GetScaledResolution() / 4);
	_VolumetricLightingResampleGraphicsPipeline.Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
															1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 4), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 4)) * 0.5f,
															RenderingSystem::Instance->GetScaledResolution() / 2,
															false);
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
	_VolumetricLightingResampleGraphicsPipeline.Execute();

	for (SeparableBlurGraphicsPipeline &pipeline : _VolumetricLightingSeparableGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_VolumetricLightingApplicationGraphicsPipeline.Execute();
	_VolumetricLightingRayTracingPipeline.Execute();
}