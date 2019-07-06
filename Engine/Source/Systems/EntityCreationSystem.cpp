//Header file.
#include <Systems/EntityCreationSystem.h>

//Multithreading.
#include <Multithreading/ScopedWriteLock.h>

//Singleton definition.
DEFINE_SINGLETON(EntityCreationSystem);

/*
*	Updates the entity creation system during the post update phase.
*/
void EntityCreationSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Process the initialization queue.
	ProcessInitializationQueue();

	//Process the termination queue.
	ProcessTerminationQueue();

	//Process the destruction queue.
	ProcessDestructionQueue();

	//Process the automatic termination queue.
	ProcessAutomaticTerminationQueue();

	//Process the automatic destruction queue.
	ProcessAutomaticDestructionQueue();
}

/*
*	Initializes one entity.
*/
void EntityCreationSystem::InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AutomaticTermination))
	{
		ScopedWriteLock<Spinlock> scopedLock{ _AutomaticTerminationQueueLock };

		_AutomaticTerminationQueue.EmplaceSlow(entity);
	}

	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AutomaticDestruction))
	{
		ScopedWriteLock<Spinlock> scopedLock{ _AutomaticDestructionQueueLock };

		_AutomaticDestructionQueue.EmplaceSlow(entity);
	}

	//Initialize this entity.
	entity->Initialize(data);

	//Set this entity to initialized.
	entity->_Initialized = true;
}

/*
*	Terminates one entity.
*/
void EntityCreationSystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate this entity.
	entity->Terminate();

	//Set this entity to un-initialized.
	entity->_Initialized = false;
}

/*
*	Destroys one entity.
*/
void EntityCreationSystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	_AllocatorLock.WriteLock();
	_Allocator.DeAllocate(entity);
	_AllocatorLock.WriteUnlock();

	//Remove the entity from the list of entities.
	_EntitiesLock.WriteLock();
	_Entities.Erase(entity);
	_EntitiesLock.WriteUnlock();
}

/*
*	Requests the initialization of en entity.
*	Initialization will happen at the next synchronous update of the entity system.
*	Usually only one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*	So if N entities are forced for the next entity system update, all of them will be initialized.
*/
void EntityCreationSystem::RequestInitialization(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data, const bool force) NOEXCEPT
{
	ASSERT(!entity->_Initialized, "Don't call EntityCreationSystem::RequestInitialization() on entities that already are initialized!");

	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//Add the data.
	_InitializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntityCreationSystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	ASSERT(entity->_Initialized, "Don't call EntityCreationSystem::RequestTermination() on entities that are not initialized!");

	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Add the data.
	_TerminationQueue.EmplaceSlow(entity);
}

/*
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*/
void EntityCreationSystem::RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add the data.
	_DestructionQueue.EmplaceSlow(entity);
}

/*
*	Processes the initialization queue.
*/
void EntityCreationSystem::ProcessInitializationQueue() NOEXCEPT
{
	//Lock the initialization queue.
	ScopedWriteLock<Spinlock> scopedLock{ _InitializationQueueLock };

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
		InitializationData& data{ _InitializationQueue[counter] };

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
		InitializationData *RESTRICT dataToInitialize = &_InitializationQueue.Back();

		InitializeEntity(dataToInitialize->_Entity, dataToInitialize->_Data);

		if (dataToInitialize == &_InitializationQueue.Back())
		{
			_InitializationQueue.PopFast();
		}

		else
		{
			_InitializationQueue.Erase(*dataToInitialize);
		}
	}
}

/*
*	Processes the termination queue.
*/
void EntityCreationSystem::ProcessTerminationQueue() NOEXCEPT
{
	//Lock the termination queue.
	ScopedWriteLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Terminate all entities.
	for (Entity *const RESTRICT entity : _TerminationQueue)
	{
		TerminateEntity(entity);
	}

	//Clear the termination queue.
	_TerminationQueue.ClearFast();
}

/*
*	Processes the destruction queue.
*/
void EntityCreationSystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	ScopedWriteLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Destroy all entities.
	for (Entity *const RESTRICT entity : _DestructionQueue)
	{
		DestroyEntity(entity);
	}

	//Clear the destruction queue.
	_DestructionQueue.ClearFast();
}

/*
*	Processes the automatic termination queue.
*/
void EntityCreationSystem::ProcessAutomaticTerminationQueue() NOEXCEPT
{
	for (uint64 i = 0; i < _AutomaticTerminationQueue.Size();)
	{
		Entity *const RESTRICT entity{ _AutomaticTerminationQueue[i] };

		if (entity->ShouldAutomaticallyTerminate())
		{
			TerminateEntity(entity);

			_AutomaticTerminationQueue.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Processes the automatic destruction queue.
*/
void EntityCreationSystem::ProcessAutomaticDestructionQueue() NOEXCEPT
{
	for (uint64 i = 0; i < _AutomaticDestructionQueue.Size();)
	{
		Entity *const RESTRICT entity{ _AutomaticDestructionQueue[i] };

		if (!entity->_Initialized)
		{
			DestroyEntity(entity);

			_AutomaticDestructionQueue.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}