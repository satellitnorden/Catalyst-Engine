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
	*	Destroys an entity.
	*/
	void DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT;

private:

	/*
	*	Entity creation queue item class definition.
	*/
	class EntityCreationQueueItem final
	{

	public:

		//The entity.
		Entity *RESTRICT _Entity;

		//The component configurations.
		StaticArray<ComponentInitializationData *RESTRICT, EntityConstants::MAXIMUM_NUMBER_OF_COMPONENTS_PER_ENTITY> _ComponentConfigurations;

		//The number of component configurations.
		uint64 _NumberOfComponentConfigurations;

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

	//The creation queue.
	AtomicQueue<EntityCreationQueueItem, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _CreationQueue;

	//The destruction queue.
	AtomicQueue<EntityDestructionQueueItem, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DestructionQueue;

	//The pre-processing queue.
	DynamicArray<EntityPreProcessingQueueItem *RESTRICT> _PreProcessingQueue;

	/*
	*	Generates a new entity identifier.
	*/
	NO_DISCARD EntityIdentifier GenerateEntityIdentifier() NOEXCEPT;

	/*
	*	Updates the entity system during the ENTITY update phase.
	*/
	void EntityUpdate() NOEXCEPT;

	/*
	*	Processed the creation queue.
	*/
	void ProcessCreationQueue() NOEXCEPT;

	/*
	*	Processed the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT;

};