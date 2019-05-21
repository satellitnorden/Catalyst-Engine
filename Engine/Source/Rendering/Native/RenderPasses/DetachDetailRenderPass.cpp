//Header file.
#include <Rendering/Native/RenderPasses/DetachDetailRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DetachDetailRenderPass);

/*
*	Default constructor.
*/
DetachDetailRenderPass::DetachDetailRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DetachDetail);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DetachDetailRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DetachDetailRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DetachDetailRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DetachDetailGraphicsPipeline);

	//Initialize all pipelines.
	_DetachDetailGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DetachDetailRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadDown == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute all pipelines.
	_DetachDetailGraphicsPipeline.Execute();
}