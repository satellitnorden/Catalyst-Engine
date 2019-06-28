//Header file.
#include <Rendering/Native/RenderPasses/DiffuseIrradianceRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DiffuseIrradianceRenderPass);

/*
*	Default constructor.
*/
DiffuseIrradianceRenderPass::DiffuseIrradianceRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::DiffuseIrradiance);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DiffuseIrradianceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DiffuseIrradianceRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DiffuseIrradianceRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DiffuseIrradianceRayTracingPipeline);

	//Initialize all pipelines.
	_DiffuseIrradianceRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DiffuseIrradianceRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetDiffuseIrradianceMode() == RenderingConfigurationManager::DiffuseIrradianceMode::RayTraced)
	{
		_DiffuseIrradianceRayTracingPipeline.Execute();
	}

	else
	{
		_DiffuseIrradianceRayTracingPipeline.SetIncludeInRender(false);
	}
}