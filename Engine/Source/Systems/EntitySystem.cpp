//Header file.
#include <Systems/EntitySystem.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Initializes the entity system.
*/
void EntitySystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<EntitySystem *const RESTRICT>(arguments)->EntityUpdate();
	},
	this,
	UpdatePhase::ENTITY,
	UpdatePhase::INPUT);
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AutomaticTermination))
	{
		SCOPED_LOCK(_AutomaticTerminationQueueLock);

		_AutomaticTerminationQueue.Emplace(entity);
	}

	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AutomaticDestruction))
	{
		SCOPED_LOCK(_AutomaticDestructionQueueLock);

		_AutomaticDestructionQueue.Emplace(entity);
	}

	//Initialize this entity.
	entity->Initialize(data);

	//Set this entity to initialized.
	entity->_Initialized = true;
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate this entity.
	entity->Terminate();

	//Set this entity to un-initialized.
	entity->_Initialized = false;
}

/*
*	Destroys one entity.
*/
void EntitySystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	_AllocatorLock.Lock();
	_Allocator.DeAllocate(entity);
	_AllocatorLock.Unlock();

	//Remove the entity from the list of entities.
	_EntitiesLock.Lock();
	_Entities.Erase(entity);
	_EntitiesLock.Unlock();
}

/*
*	Requests the initialization of en entity.
*	Initialization will happen at the next synchronous update of the entity system.
*	Usually only one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*	So if N entities are forced for the next entity system update, all of them will be initialized.
*/
void EntitySystem::RequestInitialization(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data, const bool force) NOEXCEPT
{
	ASSERT(!entity->_Initialized, "Don't call EntityCreationSystem::RequestInitialization() on entities that already are initialized!");

	//Lock the queue.
	SCOPED_LOCK(_InitializationQueueLock);

	//Add the data.
	_InitializationQueue.Emplace(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	ASSERT(entity->_Initialized, "Don't call EntityCreationSystem::RequestTermination() on entities that are not initialized!");

	//Lock the queue.
	SCOPED_LOCK(_TerminationQueueLock);

	//Add the data.
	_TerminationQueue.Emplace(entity);
}

/*
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	SCOPED_LOCK(_DestructionQueueLock);

	//Add the data.
	_DestructionQueue.Emplace(entity);
}

/*
*	Updates the entity system during the entity update phase.
*/
void EntitySystem::EntityUpdate() NOEXCEPT
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
*	Processes the initialization queue.
*/
void EntitySystem::ProcessInitializationQueue() NOEXCEPT
{
	//Lock the initialization queue.
	SCOPED_LOCK(_InitializationQueueLock);

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
			_InitializationQueue.Pop();
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
void EntitySystem::ProcessTerminationQueue() NOEXCEPT
{
	//Lock the termination queue.
	SCOPED_LOCK(_TerminationQueueLock);

	//Terminate all entities.
	for (Entity *const RESTRICT entity : _TerminationQueue)
	{
		TerminateEntity(entity);
	}

	//Clear the termination queue.
	_TerminationQueue.Clear();
}

/*
*	Processes the destruction queue.
*/
void EntitySystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	SCOPED_LOCK(_DestructionQueueLock);

	//Destroy all entities.
	for (Entity *const RESTRICT entity : _DestructionQueue)
	{
		DestroyEntity(entity);
	}

	//Clear the destruction queue.
	_DestructionQueue.Clear();
}

/*
*	Processes the automatic termination queue.
*/
void EntitySystem::ProcessAutomaticTerminationQueue() NOEXCEPT
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
void EntitySystem::ProcessAutomaticDestructionQueue() NOEXCEPT
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