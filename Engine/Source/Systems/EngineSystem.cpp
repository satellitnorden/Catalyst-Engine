//Header file.
#include <Systems/EngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>

//Systems.
#include <Systems/CullingSystem.h>
#if !defined(CATALYST_FINAL)
#include <Systems/DebugRenderingSystem.h>
#endif
#include <Systems/EntityCreationSystem.h>
#include <Systems/EntityPlacementSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UpdateSystem.h>
#include <Systems/VegetationSystem.h>

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
	EntityPlacementSystem::Instance->InitializeSystem();
	LevelOfDetailSystem::Instance->InitializeSystem();
	PhysicsSystem::Instance->InitializeSystem();
	RenderingSystem::Instance->InitializeSystem(_ProjectConfiguration._RenderingConfiguration);
	TaskSystem::Instance->InitializeSystem();
	TerrainSystem::Instance->InitializeSystem(_ProjectConfiguration._TerrainConfiguration);
	VegetationSystem::Instance->InitializeSystem();

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

	/*
	*	Pre-update phase.
	*/
	CatalystPlatform::PreUpdate(&context);
	InputSystem::Instance->UpdateSystemSynchronous(&context);

	/*
	*	Update phase.
	*/
	UpdateSystem::Instance->UpdateSystemSynchronous(&context);
	PhysicsSystem::Instance->UpdateSystemSynchronous(&context);
	CullingSystem::Instance->UpdateSystemSynchronous(&context);
	LevelOfDetailSystem::Instance->UpdateSystemSynchronous(&context);
#if !defined(CATALYST_FINAL)
	DebugRenderingSystem::Instance->UpdateSystemSynchronous(&context);
#endif
	RenderingSystem::Instance->UpdateSystemSynchronous(&context);

	/*
	*	Post-update phase.
	*/
#if !defined(CATALYST_FINAL)
	DebugRenderingSystem::Instance->PostUpdateSystemSynchronous(&context);
#endif
	EntityCreationSystem::Instance->PostUpdateSystemSynchronous(&context);
	CatalystPlatform::PostUpdate(&context);

	/*
	*	Sequential update phase.
	*/
	ExecuteSequentialUpdate();
}

/*
*	Releases the engine system.
*/
void EngineSystem::ReleaseSystem() NOEXCEPT
{
	//Signal to other systems that the game should terminate.
	_ShouldTerminate = true;

	//Release the task system first so that all asynchronous tasks are finished before releasing anything else.
	TaskSystem::Instance->ReleaseSystem();

	//Release the platform.
	CatalystPlatform::Release();

	//Release all systems.
	PhysicsSystem::Instance->ReleaseSystem();
	RenderingSystem::Instance->ReleaseSystem();
}

/*
*	Executes the sequential update.
*/
void EngineSystem::ExecuteSequentialUpdate() NOEXCEPT
{
	//Update the current sequential update.
	_CurrentSequentialUpdate =	static_cast<SequentialUpdate>(UNDERLYING(_CurrentSequentialUpdate) + 1) < SequentialUpdate::NumberOfSequentialUpdates
								? static_cast<SequentialUpdate>(UNDERLYING(_CurrentSequentialUpdate) + 1)
								: static_cast<SequentialUpdate>(0);

	//Execute the sequential update.
	switch (_CurrentSequentialUpdate)
	{
		case SequentialUpdate::EntityPlacementSystem:
		{
			EntityPlacementSystem::Instance->SequentialUpdateSystemSynchronous();

			break;
		}

		case SequentialUpdate::TerrainSystem:
		{
			TerrainSystem::Instance->SequentialUpdateSystemSynchronous();

			break;
		}

		case SequentialUpdate::VegetationSystem:
		{
			VegetationSystem::Instance->SequentialUpdateSystemSynchronous();

			break;
		}
	}
}