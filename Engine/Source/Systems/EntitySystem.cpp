//Header file.
#include <Systems/EntitySystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>
#include <Entities/EntityInitializationData.h>
#include <Entities/EntityTerminationData.h>
#include <Entities/Initialization Data/TerrainInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Default constructor.
*/
EntitySystem::EntitySystem() NOEXCEPT
{

}

/*
*	Releases the entity system.
*/
void EntitySystem::ReleaseSystem() NOEXCEPT
{
	//Destroy all remaining entities.
	for (int64 i = Entity::Instances.Size() - 1; i >= 0; --i)
	{
		delete Entity::Instances[i];
	}
}

/*
*	Pre-updates the entity system synchronously.
*/
void EntitySystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Initialize entities.
	InitializeEntities();

	//Terminate entities.
	TerminateEntities();
}

/*
*	Requests the initialization of en entity.
*	Initialization will happen at the next synchronous update of the entity system.
*	Usually only one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*	So if N entities are forced for the next entity system update, all of them will be initialized.
*/
void EntitySystem::RequestInitialization(Entity* const RESTRICT entity, void* const RESTRICT data, const bool force) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ initializationQueueLock };

	//Add the data.
	initializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*	Usually only one entity is terminated at each update of the entity system.
*	But if the termination is forced, it will take priority and will be terminated on the next update.
*	So if N entities are forced for the next entity system update, all of them will be terminated.
*/
void EntitySystem::RequesTermination(Entity* const RESTRICT entity, const bool force) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ terminationQueueLock };

	//Add the data.
	terminationQueue.EmplaceSlow(entity, force);
}

/*
*	Initializes entities.
*/
void EntitySystem::InitializeEntities() NOEXCEPT
{
	//Lock the initialization queue.
	ScopedLock<Spinlock> scopedLock{ initializationQueueLock };

	//If there's none to initialize, initialize none.
	if (initializationQueue.Empty())
	{
		return;
	}

	//Iterate through all initialization request and check for the force flag.
	uint64 forceInitialized{ 0 };
	uint64 counter{ initializationQueue.Size() - 1 };

	for (uint64 i = 0, size = initializationQueue.Size(); i < size; ++i)
	{
		EntityInitializationData& data{ initializationQueue[counter] };

		if (data.force)
		{
			InitializeEntity(&data);

			++forceInitialized;

			initializationQueue.Erase(counter);
		}

		--counter;
	}

	//If none were force-initialized, just initialize one.
	if (forceInitialized == 0)
	{
		EntityInitializationData *const RESTRICT data = &initializationQueue.Back();
		InitializeEntity(data);
		initializationQueue.Pop();
	}
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	switch (data->entity->GetEntityType())
	{
		case Entity::EntityType::Terrain:
		{
			InitializeTerrainEntity(data);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			PRINT_TO_CONSOLE("Invalid entity type!");

			BREAKPOINT;
		}
#endif
	}
}

/*
*	Initializes a terrain entity.
*/
void EntitySystem::InitializeTerrainEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for the terrain entity.
	data->entity->SetComponentsIndex(ComponentManager::GetNewTerrainComponentsIndex(data->entity));

	//Initialize the terrain entity via the rendering system.
	RenderingSystem::Instance->InitializeTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(data->entity), static_cast<const TerrainInitializationData *const RESTRICT>(data->data));

	//Destroy the initialization data.
	DestroyInitializationData<TerrainInitializationData>(data->data);
}

/*
*	Terminates entities.
*/
void EntitySystem::TerminateEntities() NOEXCEPT
{
	//Lock the termination queue.
	ScopedLock<Spinlock> scopedLock{ terminationQueueLock };

	//If there's none to terminate, terminate none.
	if (terminationQueue.Empty())
	{
		return;
	}

	//Iterate through all termination requests and check for the force flag.
	uint64 forceTerminated{ 0 };
	uint64 counter{ terminationQueue.Size() - 1 };

	for (uint64 i = 0, size = terminationQueue.Size(); i < size; ++i)
	{
		EntityTerminationData& data{ terminationQueue[counter] };

		if (data.force)
		{
			TerminateEntity(&data);

			++forceTerminated;

			terminationQueue.Erase(counter);
		}

		--counter;
	}

	//If none were force-terminated, just terminate one.
	if (forceTerminated == 0)
	{
		EntityTerminationData *const RESTRICT data = &terminationQueue.Back();
		TerminateEntity(data);
		terminationQueue.Pop();
	}
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT
{
	switch (data->entity->GetEntityType())
	{
		case Entity::EntityType::Terrain:
		{
			TerminateTerrainEntity(data);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			PRINT_TO_CONSOLE("Invalid entity type!");

			BREAKPOINT;
		}
#endif
	}
}

/*
*	Terminates a terrain entity.
*/
void EntitySystem::TerminateTerrainEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT
{
	//Terminate the terrain entity via the rendering system.
	RenderingSystem::Instance->TerminateTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(data->entity));

	//Return this entities components index.
	ComponentManager::ReturnTerrainComponentsIndex(data->entity->GetComponentsIndex());
}