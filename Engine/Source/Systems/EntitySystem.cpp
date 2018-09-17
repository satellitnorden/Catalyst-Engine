//Header file.
#include <Systems/EntitySystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>
#include <Entities/InitializationData/TerrainInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Updates the entity system synchronously during the closing update phase.
*/
void EntitySystem::ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Process the initialization queue.
	ProcessInitializationQueue();

	//Process the termination queue.
	ProcessTerminationQueue();

	//Process the destruction queue.
	ProcessDestructionQueue();
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
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*	Usually only one entity is destroyed at each update of the entity system.
*	But if the destruction is forced, it will take priority and will be destroyed on the next update.
*	So if N entities are forced for the next entity system update, all of them will be destroyed.
*/
void EntitySystem::RequestDestruction(Entity *const RESTRICT entity, const bool force) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add the data.
	_DestructionQueue.EmplaceSlow(entity, force);
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT
{
	switch (entity->_Type)
	{
		case Entity::EntityType::DynamicPhysical:
		{
			InitializeDynamicPhysicalEntity(entity, data);

			break;
		}

		case Entity::EntityType::ParticleSystem:
		{
			InitializeParticleSystemEntity(entity, data);

			break;
		}

		case Entity::EntityType::Terrain:
		{
			InitializeTerrainEntity(entity, data);

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
	entity->_Initialized = true;
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	switch (entity->_Type)
	{
		case Entity::EntityType::DynamicPhysical:
		{
			TerminateDynamicPhysicalEntity(entity);

			break;
		}

		case Entity::EntityType::Terrain:
		{
			TerminateTerrainEntity(entity);

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
	entity->_Initialized = false;
}

/*
*	Destroys one entity.
*/
void EntitySystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	delete entity;

	//Remove the entity from the list of entities.
	_Entities.Erase(entity);
}

/*
*	Processes the initialization queue.
*/
void EntitySystem::ProcessInitializationQueue() NOEXCEPT
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
			InitializeEntity(data._Entity, data._Data);

			++forceInitialized;

			_InitializationQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-initialized, just initialize one.
	if (forceInitialized == 0)
	{
		EntityInitializationData *const RESTRICT data = &_InitializationQueue.Back();
		InitializeEntity(data->_Entity, data->_Data);
		_InitializationQueue.PopFast();
	}
}

/*
*	Initializes a dynamic physical entity.
*/
void EntitySystem::InitializeDynamicPhysicalEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this dynamic physical entity.
	entity->_ComponentsIndex = ComponentManager::GetNewDynamicPhysicalComponentsIndex(entity);

	//Initialize the dynamic physical entity via the rendering system.
	RenderingSystem::Instance->InitializeDynamicPhysicalEntity(entity, static_cast<const DynamicPhysicalInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<DynamicPhysicalInitializationData>(data);
}

/*
*	Initializes a particle system physical entity.
*/
void EntitySystem::InitializeParticleSystemEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this particle system entity.
	entity->_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(entity);

	//Initialize the particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(entity, static_cast<const ParticleSystemInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Initializes a terrain entity.
*/
void EntitySystem::InitializeTerrainEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for the terrain entity.
	entity->_ComponentsIndex = ComponentManager::GetNewTerrainComponentsIndex(entity);

	//Initialize the terrain entity via the rendering system.
	RenderingSystem::Instance->InitializeTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(entity), static_cast<const TerrainInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<TerrainInitializationData>(data);
}

/*
*	Processes the termination queue.
*/
void EntitySystem::ProcessTerminationQueue() NOEXCEPT
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
			TerminateEntity(data._Entity);

			++forceTerminated;

			_TerminationQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-terminated, just terminate one.
	if (forceTerminated == 0)
	{
		EntityTerminationData *const RESTRICT data = &_TerminationQueue.Back();
		TerminateEntity(data->_Entity);
		_TerminationQueue.PopFast();
	}
}

/*
*	Terminates a dynamic physical entity.
*/
void EntitySystem::TerminateDynamicPhysicalEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicPhysicalComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a terrain entity.
*/
void EntitySystem::TerminateTerrainEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate the terrain entity via the rendering system.
	RenderingSystem::Instance->TerminateTerrainEntity(reinterpret_cast<const TerrainEntity *const RESTRICT>(entity));

	//Return this entitiy's components index.
	ComponentManager::ReturnTerrainComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Processes the destruction queue.
*/
void EntitySystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//If there's none to destroy, destroy none.
	if (_DestructionQueue.Empty())
	{
		return;
	}

	//Iterate through all destruction requests and check for the force flag.
	uint64 forceDestroyed{ 0 };
	uint64 counter{ _DestructionQueue.Size() - 1 };

	for (uint64 i = 0, size = _DestructionQueue.Size(); i < size; ++i)
	{
		EntityDestructionData& data{ _DestructionQueue[counter] };

		if (data._Force)
		{
			DestroyEntity(data._Entity);

			++forceDestroyed;

			_DestructionQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-destroyed, just destroy one.
	if (forceDestroyed == 0)
	{
		EntityDestructionData &data = _DestructionQueue.Back();
		DestroyEntity(data._Entity);
		_DestructionQueue.PopFast();
	}
}