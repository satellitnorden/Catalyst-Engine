#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>
#include <Concurrency/Spinlock.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/EntityAsset.h>

//Entities.
#include <Entities/Core/Entity.h>
#include <Entities/Core/EntityLinks.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Systems.
#include <Systems/MemorySystem.h>
#include <Systems/System.h>

//World.
#include <World/Core/WorldTransform.h>

//Forward declarations.
class ComponentInitializationData;

class EntitySystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		EntitySystem
	);

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
	NO_DISCARD Entity *const RESTRICT CreateEntity(const WorldTransform &world_transform, AssetPointer<EntityAsset> entity) NOEXCEPT;

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

	/*
	*	Returns the entity links.
	*/
	FORCE_INLINE NO_DISCARD EntityLinks *const RESTRICT GetEntityLinks() NOEXCEPT
	{
		return &_EntityLinks;
	}

private:

	//Constants.
	constexpr static uint64 CREATION_QUEUE_SIZE{ 8'192 };
	constexpr static uint64 DESTRUCTION_QUEUE_SIZE{ 8'192 };

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
	AtomicQueue<EntityDestructionQueueItem, DESTRUCTION_QUEUE_SIZE, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DestructionQueue;

	//The pre-processing queue.
	DynamicArray<EntityPreProcessingQueueItem *RESTRICT> _PreProcessingQueue;

	//Container for all entities.
	DynamicArray<Entity *RESTRICT> _Entities;

	//The entity links.
	EntityLinks _EntityLinks;

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