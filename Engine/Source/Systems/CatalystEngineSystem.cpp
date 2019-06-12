//Header file.
#include <Systems/CatalystEngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>
#include <Core/General/DeltaTimer.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UserInterfaceSystem.h>

namespace CatalystEngineSystemInternalData
{
	//The delta timer.
	DeltaTimer _DeltaTimer;
}

/*
*	Initializes the Catalyst engine system.
*/
void CatalystEngineSystem::Initialize(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT
{
	//Set the project configuration.
	ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration = initialProjectConfiguration;

	//Initialize the platform.
	CatalystPlatform::Initialize();

	//Initialize all systems.
	RenderingSystem::Instance->Initialize(ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._RenderingConfiguration);
	SoundSystem::Instance->Initialize();
	TaskSystem::Instance->Initialize();

	//Post-initialize all systems.
	RenderingSystem::Instance->PostInitializeSystem();

	//Post-initialize the platform.
	CatalystPlatform::PostInitialize();

	//Initialize the game system.
	ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._InitializationFunction();
}

/*
*	Updates the Catalyst engine system.
*/
bool CatalystEngineSystem::Update() NOEXCEPT
{
	//Update the total frames.
	++ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_TotalFrames;

	//Update the total time.
	ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_TotalTime += ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_DeltaTime;

	//Update the delta time.
	ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_DeltaTime = CatalystEngineSystemInternalData::_DeltaTimer.Update();

	//Construct the update context.
	UpdateContext context;

	context._TotalFrames = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_TotalFrames;
	context._TotalTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_TotalTime;
	context._DeltaTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_DeltaTime;

	/*
	*	Pre update phase.
	*/
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._PreUpdateFunction(&context);

	CatalystPlatform::PreUpdate(&context);

	InputSystem::PreUpdate(&context);

	/*
	*	Logic update phase.
	*/
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._LogicUpdateFunction(&context);

	/*
	*	Physics update phase.
	*/
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._PhysicsUpdateFunction(&context);
	
	PhysicsSystem::PhysicsUpdate(&context);

	/*
	*	Render update phase.
	*/
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._RenderUpdateFunction(&context);

	RenderingSystem::Instance->RenderUpdate(&context);

	/*
	*	Post-update phase.
	*/
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._PostUpdateFunction(&context);

	CatalystPlatform::PostUpdate(&context);

	EntityCreationSystem::Instance->PostUpdate(&context);
	SoundSystem::Instance->PostUpdate(&context);

	//Return if the game should be terminated.
	return !ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ShouldTerminate;
}

/*
*	Releases the Catalyst engine system.
*/
void CatalystEngineSystem::Terminate() NOEXCEPT
{
	//Signal to other systems that the game should terminate.
	ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_ShouldTerminate = true;

	//Terminate the game system.
	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._TerminationFunction();

	//Terminate the task system first so that all asynchronous tasks are finished before releasing anything else.
	TaskSystem::Instance->Terminate();

	//Release the platform.
	CatalystPlatform::Release();

	//Release all systems.
	RenderingSystem::Instance->Release();
	SoundSystem::Instance->Terminate();
	UserInterfaceSystem::Terminate();
}