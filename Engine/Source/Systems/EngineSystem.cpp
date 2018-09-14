//Header file.
#include <Systems/EngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>

//Systems.
#include <Systems/CullingSystem.h>
#if !defined(CATALYST_FINAL)
#include <Systems/DebugRenderingSystem.h>
#endif
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EngineSystem);

/*
*	Initializes the engine system.
*/
void EngineSystem::InitializeSystem(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT
{
	//Set the project configuration.
	_ProjectConfiguration = initialProjectConfiguration;

	//Initialize the platform.
	CatalystPlatform::Initialize();

	//Initialize all systems.
	InputSystem::Instance->InitializeSystem();
	RenderingSystem::Instance->InitializeSystem(_ProjectConfiguration._RenderingConfiguration);
	TaskSystem::Instance->InitializeSystem();

	//Post-initialize all systems.
	RenderingSystem::Instance->PostInitializeSystem();

	//Post-initialize the platform.
	CatalystPlatform::PostInitialize();
}

/*
*	Updates the engine system synchronously.
*/
void EngineSystem::UpdateSystemSynchronous(const float newDeltaTime) NOEXCEPT
{
	//Update the delta time.
	_DeltaTime = newDeltaTime;

	//Update the total time.
	_TotalTime += _DeltaTime;

	//Construct the update context.
	UpdateContext context;
	context._DeltaTime = _DeltaTime;
	context._TotalTime = _TotalTime;

	//Pre-update the platform.
	CatalystPlatform::PreUpdate(&context);

	/*
	*	Opening update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::OpeningUpdate;
	UpdateSystem::Instance->PreOpeningUpdateSystemSynchronous(&context);
	InputSystem::Instance->PreOpeningUpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostOpeningUpdateSystemSynchronous(&context);
	InputSystem::Instance->PostOpeningUpdateSystemSynchronous(&context);

	/*
	*	Logic update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::LogicUpdate;
	UpdateSystem::Instance->PreLogicUpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostLogicUpdateSystemSynchronous(&context);

	/*
	*	Physics update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::PhysicsUpdate;
	UpdateSystem::Instance->PrePhysicsUpdateSystemSynchronous(&context);
	PhysicsSystem::Instance->PhysicsUpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostPhysicsUpdateSystemSynchronous(&context);

	/*
	*	Rendering update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::RenderingUpdate;
	UpdateSystem::Instance->PreRenderingUpdateSystemSynchronous(&context);
#if !defined(CATALYST_FINAL)
	DebugRenderingSystem::Instance->RenderingUpdateSystemSynchronous(&context);
#endif
	RenderingSystem::Instance->RenderingUpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostRenderingUpdateSystemSynchronous(&context);

	/*
	*	Closing update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::ClosingUpdate;
	UpdateSystem::Instance->PreClosingUpdateSystemSynchronous(&context);
	EntitySystem::Instance->ClosingUpdateSystemSynchronous(&context);
#if !defined(CATALYST_FINAL)
	DebugRenderingSystem::Instance->ClosingUpdateSystemSynchronous(&context);
#endif
	UpdateSystem::Instance->PostClosingUpdateSystemSynchronous(&context);

	//Post-update the platform.
	CatalystPlatform::PostUpdate(&context);
}

/*
*	Releases the engine system.
*/
void EngineSystem::ReleaseSystem() NOEXCEPT
{
	//Signal to other systems that the game should terminate.
	_ShouldTerminate = true;

	//Release the platform.
	CatalystPlatform::Release();

	//Release all systems.
	EntitySystem::Instance->ReleaseSystem();
	RenderingSystem::Instance->ReleaseSystem();
	TaskSystem::Instance->ReleaseSystem();
}