//Header file.
#include <Systems/EntitySystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/EntityInitializationData.h>
#include <Entities/EntityTerminationData.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/TerrainInitializationData.h>

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
*	Updates the entity system synchronously.
*/
void EntitySystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Initialize entities.
	InitializeEntities();

	//Terminate entities.
	TerminateEntities();
}

/*
*	Releases the entity system.
*/
void EntitySystem::ReleaseSystem() NOEXCEPT
{
	//Destroy all remaining entities.
	for (int64 i = _Entities.Size() - 1; i >= 0; --i)
	{
		delete _Entities[i];
	}
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
	ScopedLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//Add the data.
	_InitializationQueue.EmplaceSlow(entity, data, force);
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
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Add the data.
	_TerminationQueue.EmplaceSlow(entity, force);
}

/*
*	Initializes entities.
*/
void EntitySystem::InitializeEntities() NOEXCEPT
{
	//Lock the initialization queue.
	ScopedLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//If there's none to initialize, initialize none.
	if (_InitializationQueue.Empty())
	{
		return;
	}

	//Iterate through all initialization request and check for the force flag.
	uint64 forceInitialized{ 0 };
	uint64 counter{ _InitializationQueue.Size() - 1 };

	for (uint64 i = 0, size = _InitializationQueue.Size(); i < size; ++i)
	{
		EntityInitializationData& data{ _InitializationQueue[counter] };

		if (data._Force)
		{
			InitializeEntity(&data);

			++forceInitialized;

			_InitializationQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-initialized, just initialize one.
	if (forceInitialized == 0)
	{
		EntityInitializationData *const RESTRICT data = &_InitializationQueue.Back();
		InitializeEntity(data);
		_InitializationQueue.PopFast();
	}
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	switch (data->_Entity->GetEntityType())
	{
		case Entity::EntityType::DynamicPhysical:
		{
			InitializeDynamicPhysicalEntity(data);

			break;
		}

		case Entity::EntityType::Terrain:
		{
			InitializeTerrainEntity(data);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			ASSERT(false, "Invalid entity type!");
		}
#endif
	}

	//Set this entity to initialized.
	data->_Entity->SetIsInitialized(true);
}

/*
*	Initializes a dynamic physical entity.
*/
void EntitySystem::InitializeDynamicPhysicalEntity(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for the dynamic physical entity.
	data->_Entity->SetComponentsIndex(ComponentManager::GetNewDynamicPhysicalComponentsIndex(data->_Entity));

	//Initialize the dynamic physical entity via the rendering system.
	RenderingSystem::Instance->InitializeDynamicPhysicalEntity(data->_Entity, static_cast<const DynamicPhysicalInitializationData *const RESTRICT>(data->_Data));

	//Destroy the initialization data.
	DestroyInitializationData<DynamicPhysicalInitializationData>(data->_Data);
}

/*
*	Initializes a terrain entity.
*/
void EntitySystem::InitializeTerrainEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for the terrain entity.
	data->_Entity->SetComponentsIndex(ComponentManager::GetNewTerrainComponentsIndex(data->_Entity));

	//Initialize the terrain entity via the rendering system.
	RenderingSystem::Instance->InitializeTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(data->_Entity), static_cast<const TerrainInitializationData *const RESTRICT>(data->_Data));

	//Destroy the initialization data.
	DestroyInitializationData<TerrainInitializationData>(data->_Data);
}

/*
*	Terminates entities.
*/
void EntitySystem::TerminateEntities() NOEXCEPT
{
	//Lock the termination queue.
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//If there's none to terminate, terminate none.
	if (_TerminationQueue.Empty())
	{
		return;
	}

	//Iterate through all termination requests and check for the force flag.
	uint64 forceTerminated{ 0 };
	uint64 counter{ _TerminationQueue.Size() - 1 };

	for (uint64 i = 0, size = _TerminationQueue.Size(); i < size; ++i)
	{
		EntityTerminationData& data{ _TerminationQueue[counter] };

		if (data._Force)
		{
			TerminateEntity(&data);

			++forceTerminated;

			_TerminationQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-terminated, just terminate one.
	if (forceTerminated == 0)
	{
		EntityTerminationData *const RESTRICT data = &_TerminationQueue.Back();
		TerminateEntity(data);
		_TerminationQueue.PopFast();
	}
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT
{
	switch (data->_Entity->GetEntityType())
	{
		case Entity::EntityType::Terrain:
		{
			TerminateTerrainEntity(data);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			ASSERT(false, "Invalid entity type!");
		}
#endif
	}

	//Set this entity to un-initialized.
	data->_Entity->SetIsInitialized(false);
}

/*
*	Terminates a terrain entity.
*/
void EntitySystem::TerminateTerrainEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT
{
	//Terminate the terrain entity via the rendering system.
	RenderingSystem::Instance->TerminateTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(data->_Entity));

	//Return this entities components index.
	ComponentManager::ReturnTerrainComponentsIndex(data->_Entity->GetComponentsIndex());
}