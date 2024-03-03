#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>
#include <Concurrency/Spinlock.h>

//Entities.
#include <Entities/Types/Entity.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Systems.
#include <Systems/MemorySystem.h>

class ALIGN(8) EntitySystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntitySystem);

	/*
	*	Default constructor.
	*/
	EntitySystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the entity system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Creates an entity.
	*/
	NO_DISCARD Entity *const RESTRICT CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT> component_configurations) NOEXCEPT;

	/*
	*	Adds a component to the given entity.
	*/
	void AddComponentToEntity(Entity *const RESTRICT entity, ComponentInitializationData *RESTRICT component_configuration) NOEXCEPT;

	/*
	*	Destroys an entity.
	*/
	void DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Destroys all entities.
	*/
	void DestroyAllEntities() NOEXCEPT;

private:

	//Constants.
	constexpr static uint64 CREATION_QUEUE_SIZE{ 4'096 };
	constexpr static uint64 CREATION_QUEUE_THRESHOLD{ 3'072 }; //3/4 of the size.

	/*
	*	Entity creation queue item class definition.
	*/
	class EntityCreationQueueItem final
	{

	public:

		//The entity.
		Entity *RESTRICT _Entity;

		//The component configurations.
		ComponentInitializationData *RESTRICT _ComponentConfigurations;

	};

	/*
	*	Entity destruction queue item class definition.
	*/
	class EntityDestructionQueueItem final
	{

	public:

		//The entity.
		Entity *RESTRICT _Entity;

	};

	/*
	*	Entity pre-processing queue item.
	*/
	class EntityPreProcessingQueueItem final
	{

	public:

		//The creation queue item.
		EntityCreationQueueItem _CreationQueueItem;

		//The task.
		Task _Task;

	};

	//The entity allocator lock.
	Spinlock _EntityAllocatorLock;

	//The allocator.
	PoolAllocator<sizeof(Entity)> _EntityAllocator;

	//The pre-processing allocator.
	PoolAllocator<sizeof(EntityPreProcessingQueueItem)> _PreProcessingAllocator;

	//The entity identifier lock.
	Spinlock _EntityIdentifierLock;

	//The entity identifier counter.
	EntityIdentifier _EntityIdentifierCounter{ 0 };

	//The entity identifier free list.
	DynamicArray<EntityIdentifier> _EntityIdentifierFreeList;

	//The number of items in the creation queue.
	Atomic<uint64> _NumberOfItemsInCreationQueue{ 0 };

	//The creation queue.
	AtomicQueue<EntityCreationQueueItem, CREATION_QUEUE_SIZE, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _CreationQueue;

	//The destruction queue.
	AtomicQueue<EntityDestructionQueueItem, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DestructionQueue;

	//The pre-processing queue.
	DynamicArray<EntityPreProcessingQueueItem *RESTRICT> _PreProcessingQueue;

	//Container for all entities.
	DynamicArray<Entity *const RESTRICT> _Entities;

	/*
	*	Generates a new entity identifier.
	*/
	NO_DISCARD EntityIdentifier GenerateEntityIdentifier() NOEXCEPT;

	/*
	*	Updates the entity system during the ENTITY update phase.
	*/
	void EntityUpdate() NOEXCEPT;

	/*
	*	Processes the creation queue.
	*/
	void ProcessCreationQueue() NOEXCEPT;

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT;

};