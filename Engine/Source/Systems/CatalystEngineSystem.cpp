//Header file.
#include <Systems/CatalystEngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>
#include <Core/General/DeltaTimer.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Systems/DebugRenderingSystem.h>
#endif
#include <Systems/EntityCreationSystem.h>
#include <Systems/EntityPlacementSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/VegetationSystem.h>

namespace CatalystEngineSystemInternalData
{
	//Enumeration covering all sequential updates.
	enum class SequentialUpdate : uint8
	{
		EntityPlacementSystem,
		TerrainSystem,
		VegetationSystem,

		NumberOfSequentialUpdates
	};


	//The delta timer.
	DeltaTimer _DeltaTimer;

	//The current sequential update.
	SequentialUpdate _CurrentSequentialUpdate{ SequentialUpdate::NumberOfSequentialUpdates };
}

namespace CatalystEngineSystemInternalLogic
{
	/*
	*	Executes the sequential update.
	*/
	void ExecuteSequentialUpdate() NOEXCEPT
	{
		//Update the current sequential update.
		CatalystEngineSystemInternalData::_CurrentSequentialUpdate =	static_cast<CatalystEngineSystemInternalData::SequentialUpdate>(UNDERLYING(CatalystEngineSystemInternalData::_CurrentSequentialUpdate) + 1) < CatalystEngineSystemInternalData::SequentialUpdate::NumberOfSequentialUpdates
																		? static_cast<CatalystEngineSystemInternalData::SequentialUpdate>(UNDERLYING(CatalystEngineSystemInternalData::_CurrentSequentialUpdate) + 1)
																		: static_cast<CatalystEngineSystemInternalData::SequentialUpdate>(0);

		//Execute the sequential update.
		switch (CatalystEngineSystemInternalData::_CurrentSequentialUpdate)
		{
			case CatalystEngineSystemInternalData::SequentialUpdate::EntityPlacementSystem:
			{
				EntityPlacementSystem::Instance->SequentialUpdateSystemSynchronous();

				break;
			}

			case CatalystEngineSystemInternalData::SequentialUpdate::TerrainSystem:
			{
				TerrainSystem::Instance->SequentialUpdateSystemSynchronous();

				break;
			}

			case CatalystEngineSystemInternalData::SequentialUpdate::VegetationSystem:
			{
				VegetationSystem::Instance->SequentialUpdateSystemSynchronous();

				break;
			}
		}
	}
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
	EntityPlacementSystem::Instance->InitializeSystem();
	PhysicsSystem::Instance->Initialize();
	RenderingSystem::Instance->Initialize(ComponentManager::WriteSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._RenderingConfiguration);
	SoundSystem::Instance->Initialize();
	TaskSystem::Instance->InitializeSystem();
	TerrainSystem::Instance->InitializeSystem();
	VegetationSystem::Instance->InitializeSystem();

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
	*	Pre-update phase.
	*/
	*CurrentUpdatePhase() = UpdatePhase::Pre;

	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._PreUpdateFunction(&context);
	CatalystPlatform::PreUpdate(&context);
	InputSystem::Update(&context);

	/*
	*	Main update phase.
	*/
	*CurrentUpdatePhase() = UpdatePhase::Main;

	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._UpdateFunction(&context);
	PhysicsSystem::Instance->Update(&context);
#if defined(CATALYST_CONFIGURATION_DEBUG)
	DebugRenderingSystem::Instance->UpdateSystemSynchronous(&context);
#endif
	RenderingSystem::Instance->UpdateSystem(&context);
	SoundSystem::Instance->Update(&context);

	/*
	*	Post-update phase.
	*/
	*CurrentUpdatePhase() = UpdatePhase::Post;

	ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_ProjectConfiguration._GeneralConfiguration._PostUpdateFunction(&context);
#if defined(CATALYST_CONFIGURATION_DEBUG)
	DebugRenderingSystem::Instance->PostUpdateSystemSynchronous(&context);
#endif
	EntityCreationSystem::Instance->PostUpdateSystemSynchronous(&context);
	CatalystPlatform::PostUpdate(&context);

	/*
	*	Sequential update phase.
	*/
	CatalystEngineSystemInternalLogic::ExecuteSequentialUpdate();

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

	//Release the task system first so that all asynchronous tasks are finished before releasing anything else.
	TaskSystem::Instance->ReleaseSystem();

	//Release the platform.
	CatalystPlatform::Release();

	//Release all systems.
	PhysicsSystem::Instance->Terminate();
	RenderingSystem::Instance->Release();
	SoundSystem::Instance->Terminate();
	UserInterfaceSystem::Terminate();
}