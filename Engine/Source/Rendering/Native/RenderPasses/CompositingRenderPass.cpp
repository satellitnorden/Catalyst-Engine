//Header file.
#include <Rendering/Native/RenderPasses/CompositingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CompositingRenderPass);

/*
*	Default constructor.
*/
CompositingRenderPass::CompositingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::Compositing);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		CompositingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		CompositingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void CompositingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_VolumetricLightingResampleGraphicsPipeline);
	AddPipeline(&_CompositingGraphicsPipeline);

	//Initialize all pipelines.
	_VolumetricLightingResampleGraphicsPipeline.Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::VolumetricLighting),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float),
															1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 4), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 4)) * 0.5f,
															1,						
															RenderingSystem::Instance->GetScaledResolution() / 2,
															false);
	_CompositingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void CompositingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_VolumetricLightingResampleGraphicsPipeline.Execute();
	_CompositingGraphicsPipeline.Execute();
}