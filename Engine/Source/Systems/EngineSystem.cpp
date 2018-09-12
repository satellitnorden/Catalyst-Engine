//Header file.
#include <Systems/EngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>

//Systems.
#include <Systems/CullingSystem.h>
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
	CullingSystem::Instance->InitializeSystem();
	LevelOfDetailSystem::Instance->InitializeSystem();
	RenderingSystem::Instance->InitializeSystem(_ProjectConfiguration._RenderingConfiguration);
	SoundSystem::Instance->InitializeSystem();
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

	/*
	*	Pre-update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::PreUpdate;
	CatalystPlatform::PreUpdate(&context);
	UpdateSystem::Instance->PreUpdateSystemSynchronous(&context);
	CullingSystem::Instance->PreUpdateSystemSynchronous(&context);
	InputSystem::Instance->PreUpdateSystemSynchronous(&context);

	/*
	*	Update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::Update;
	UpdateSystem::Instance->UpdateSystemSynchronous(&context);
	EntitySystem::Instance->UpdateSystemSynchronous(&context);
	PhysicsSystem::Instance->UpdateSystemSynchronous(&context);
	LevelOfDetailSystem::Instance->UpdateSystemSynchronous(&context);
	RenderingSystem::Instance->UpdateSystemSynchronous(&context);

	/*
	*	Post-update phase.
	*/
	_CurrentUpdatePhase = UpdatePhase::PostUpdate;
	UpdateSystem::Instance->PostUpdateSystemSynchronous(&context);
	RenderingSystem::Instance->PostUpdateSystemSynchronous(&context);
	SoundSystem::Instance->PostUpdateSystemSynchronous(&context);
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
	SoundSystem::Instance->ReleaseSystem();
	TaskSystem::Instance->ReleaseSystem();
}