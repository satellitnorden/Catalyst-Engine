//Header file.
#include <Systems/EngineSystem.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

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
	InputSystem::Instance->InitializeSystem();
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

	//Update the total game time.
	_TotalGameTime += _DeltaTime;

	//Pre-update the platform.
	CatalystPlatform::PreUpdate();

	//Pre-update the input system.
	InputSystem::Instance->PreUpdateSystemSynchronous();

	//Pre-update the entity system.
	EntitySystem::Instance->PreUpdateSystemSynchronous();

	//Pre-update the rendering system.
	RenderingSystem::Instance->PreUpdateSystemSynchronous();

	//Update the culling system.
	CullingSystem::Instance->UpdateSystemSynchronous();

	//Update the level of detail system.
	LevelOfDetailSystem::Instance->UpdateSystemSynchronous();

	//Update the physics system.
	PhysicsSystem::Instance->UpdateSystemSynchronous(_DeltaTime);

	//Update the graphics system.
	RenderingSystem::Instance->UpdateSystemSynchronous();

	//Update the sound system.
	SoundSystem::Instance->UpdateSystemSynchronous(_DeltaTime);

	//Post-update the input system.
	InputSystem::Instance->PostUpdateSystemSynchronous();

	//Post-update the platform.
	CatalystPlatform::PostUpdate();
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