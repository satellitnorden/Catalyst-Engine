//Header file.
#include <Systems/EntityCreationSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntityCreationSystem);

/*
*	Updates the entity creation system synchronously during the closing update phase.
*/
void EntityCreationSystem::ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
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
	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticDestruction))
	{
		ScopedLock<Spinlock> scopedLock{ _AutomaticDestructionQueueLock };

		_AutomaticDestructionQueue.EmplaceSlow(entity);
	}

	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticTermination))
	{
		ScopedLock<Spinlock> scopedLock{ _AutomaticTerminationQueueLock };

		_AutomaticTerminationQueue.EmplaceSlow(entity);
	}

	//Initialize this entity in different ways depending on the entity type.
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
void EntityCreationSystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	switch (entity->_Type)
	{
		case Entity::EntityType::DynamicPhysical:
		{
			TerminateDynamicPhysicalEntity(entity);

			break;
		}

		case Entity::EntityType::ParticleSystem:
		{
			TerminateParticleSystemEntity(entity);

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
void EntityCreationSystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	_Allocator.DeAllocate(entity);

	//Remove the entity from the list of entities.
	_Entities.Erase(entity);
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
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//Add the data.
	_InitializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntityCreationSystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

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
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add the data.
	_DestructionQueue.EmplaceSlow(entity);
}

/*
*	Processes the initialization queue.
*/
void EntityCreationSystem::ProcessInitializationQueue() NOEXCEPT
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
		InitializationData *const RESTRICT data = &_InitializationQueue.Back();
		InitializeEntity(data->_Entity, data->_Data);
		_InitializationQueue.PopFast();
	}
}

/*
*	Initializes a dynamic physical entity.
*/
void EntityCreationSystem::InitializeDynamicPhysicalEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
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
void EntityCreationSystem::InitializeParticleSystemEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this particle system entity.
	entity->_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(entity);

	//Initialize the particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(entity, static_cast<const ParticleSystemInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Processes the termination queue.
*/
void EntityCreationSystem::ProcessTerminationQueue() NOEXCEPT
{
	//Lock the termination queue.
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Terminate all entities.
	for (Entity *const RESTRICT entity : _TerminationQueue)
	{
		TerminateEntity(entity);
	}

	//Clear the termination queue.
	_TerminationQueue.ClearFast();
}

/*
*	Terminates a dynamic physical entity.
*/
void EntityCreationSystem::TerminateDynamicPhysicalEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicPhysicalComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a particle system entity.
*/
void EntityCreationSystem::TerminateParticleSystemEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate the particle system entity via the rendering system.
	RenderingSystem::Instance->TerminateParticleSystemEntity(entity);

	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Processes the destruction queue.
*/
void EntityCreationSystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

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