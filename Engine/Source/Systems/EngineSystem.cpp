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
	LevelOfDetailSystem::Instance->InitializeSystem();
	RenderingSystem::Instance->InitializeSystem(_ProjectConfiguration._RenderingConfiguration);
	TaskSystem::Instance->InitializeSystem();

	//Post-initialize all systems.
	LevelOfDetailSystem::Instance->PostInitializeSystem();
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
	EntitySystem::Instance->UpdateSystemSynchronous(&context);
	PhysicsSystem::Instance->UpdateSystemSynchronous(&context);
	LevelOfDetailSystem::Instance->UpdateSystemSynchronous(&context);
	RenderingSystem::Instance->UpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostLogicUpdateSystemSynchronous(&context);

	/*
	*	Render update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::RenderUpdate;
	UpdateSystem::Instance->PreRenderUpdateSystemSynchronous(&context);
#if !defined(CATALYST_FINAL)
	DebugRenderingSystem::Instance->RenderUpdateSystemSynchronous(&context);
#endif
	RenderingSystem::Instance->RenderUpdateSystemSynchronous(&context);
	UpdateSystem::Instance->PostRenderUpdateSystemSynchronous(&context);

	/*
	*	Closing update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::ClosingUpdate;
	UpdateSystem::Instance->PreClosingUpdateSystemSynchronous(&context);
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