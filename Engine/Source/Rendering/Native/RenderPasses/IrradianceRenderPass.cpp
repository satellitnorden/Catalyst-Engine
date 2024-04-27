//Header file.
#include <Rendering/Native/RenderPasses/IrradianceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IrradianceRenderPass);

/*
*	Default constructor.
*/
IrradianceRenderPass::IrradianceRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::IRRADIANCE, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IrradianceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IrradianceRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		IrradianceRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void IrradianceRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);

	AddPipeline(&_RayTracedDiffuseIrradiancePipeline);

	//Initialize all pipelines.
	_RayTracedDiffuseIrradiancePipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void IrradianceRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetDiffuseIrradianceMode())
	{
		case RenderingConfiguration::DiffuseIrradianceMode::NONE:
		{
			_RayTracedDiffuseIrradiancePipeline.SetIncludeInRender(false);

			break;
		}

		case RenderingConfiguration::DiffuseIrradianceMode::RAY_TRACED:
		{
			_RayTracedDiffuseIrradiancePipeline.Execute();
			_RayTracedDiffuseIrradiancePipeline.SetIncludeInRender(true);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	
}

/*
*	Terminates this render pass.
*/
void IrradianceRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_RayTracedDiffuseIrradiancePipeline.Terminate();
}