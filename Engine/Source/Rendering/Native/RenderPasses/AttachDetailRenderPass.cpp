//Header file.
#include <Rendering/Native/RenderPasses/AttachDetailRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AttachDetailRenderPass);

/*
*	Default constructor.
*/
AttachDetailRenderPass::AttachDetailRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::AttachDetail);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		AttachDetailRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AttachDetailRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void AttachDetailRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_AttachDetailGraphicsPipeline);

	//Initialize all pipelines.
	_AttachDetailGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void AttachDetailRenderPass::Execute() NOEXCEPT
{	
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadDown == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute all pipelines.
	_AttachDetailGraphicsPipeline.Execute();
}