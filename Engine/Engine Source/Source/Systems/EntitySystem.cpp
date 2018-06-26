//Header file.
#include <Systems/EntitySystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>
#include <Entities/Initialization Data/EntityInitializationData.h>
#include <Entities/Initialization Data/TerrainInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

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
}

/*
*	Requests the initialization of en entity.
*	Usually one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*/
void EntitySystem::RequestInitialization(Entity* const RESTRICT entity, void* const RESTRICT data, const bool force) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock>{initializationQueueLock};

	//Add the data.
	initializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Initializes entities.
*/
void EntitySystem::InitializeEntities() NOEXCEPT
{
	//If there's none to initialize, initialize none.
	if (initializationQueue.Empty())
	{
		return;
	}

	//Lock the initialization queue.
	ScopedLock<Spinlock>{ initializationQueueLock };

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
	data->entity->SetComponentsIndex(ComponentManager::GetNewTerrainComponentsIndex());

	//Initialize the terrain entity via the rendering system.
	RenderingSystem::Instance->InitializeTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(data->entity), static_cast<const TerrainInitializationData *const RESTRICT>(data->data));

	//Destroy the initialization data.
	DestroyInitializationData<TerrainInitializationData>(data->data);
}