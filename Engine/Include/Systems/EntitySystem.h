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
#include <Entities/Creation/EntityInitializationData.h>
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

	///////////////
	// NEW SETUP //
	///////////////

	/*
	*	Creates an entity.
	*/
	NO_DISCARD Entity *const RESTRICT CreateEntity(ArrayProxy<ComponentInitializationData *const RESTRICT> component_configurations) NOEXCEPT;

	/*
	*	Destroys an entity.
	*/
	void DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT;

	///////////////
	// OLD SETUP //
	///////////////

	/*
	*	Creates a new entity.
	*/
	template <class CLASS, class... ARGUMENTS>
	RESTRICTED CLASS *const RESTRICT CreateEntity(ARGUMENTS&&... arguments) NOEXCEPT;

	/*
	*	Creates initialization data for an entity.
	*/
	template <typename TYPE>
	RESTRICTED TYPE *const RESTRICT CreateInitializationData() NOEXCEPT;

	/*
	*	Destroys initialization data for an entity.
	*/
	template <typename TYPE>
	void DestroyInitializationData(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes one entity.
	*/
	void InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates one entity.
	*/
	void TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT;

	/*
	*	Destroys one entity.
	*/
	void DestroyEntityOld(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Requests the initialization of en entity.
	*	Initialization will happen at the next synchronous update of the entity system.
	*	Usually only one entity is initialized at each update of the entity system.
	*	But if the initialization is forced, it will take priority and will be initialized on the next update.
	*	So if N entities are forced for the next entity system update, all of them will be initialized.
	*/
	void RequestInitialization(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data, const bool force) NOEXCEPT;

	/*
	*	Requests the termination of en entity.
	*	Termination will happen at the next synchronous update of the entity system.
	*/
	void RequestTermination(Entity* const RESTRICT entity) NOEXCEPT;

	/*
	*	Requests the destruction of an entity.
	*	Destruction will happen at the next synchronous update of the entity system.
	*/
	void RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Requests the duplication of the given entity.
	*	Initialization will happen at the next synchronous update of the entity system.
	*/
	RESTRICTED NO_DISCARD Entity *const RESTRICT DuplicateEntity(const Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates/destroys all entities.
	*/
	void DestroyAllEntities() NOEXCEPT;

private:

	///////////////
	// NEW SETUP //
	///////////////

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

	///////////////
	// OLD SETUP //
	///////////////

	/*
	*	Initialization data definition.
	*/
	class InitializationData final
	{

	public:

		/*
		*	Preprocessing data class definition.
		*/
		class PreprocessingData final
		{

		public:

			//The task.
			Task _Task;

			//The entity to initialize.
			Entity* RESTRICT _Entity;

			//The initialization data.
			EntityInitializationData* RESTRICT _Data;
		};

		//The entity to initialize.
		Entity *RESTRICT _Entity;

		//The initialization data.
		EntityInitializationData *RESTRICT _Data;

		//Denotes whether or not to force this initialization.
		bool _Force;

		//The preprocessing data.
		PreprocessingData *RESTRICT _PreprocessingData{ nullptr };

		/*
		*	Default constructor, prohibited - must be constructed with the proper arguments.
		*/
		InitializationData() NOEXCEPT = delete;

		/*
		*	Constructor taking all values as arguments.
		*/
		InitializationData(Entity* const RESTRICT initialEntity, EntityInitializationData* const RESTRICT initialData, const bool initialForce) NOEXCEPT
			:
			_Entity(initialEntity),
			_Data(initialData),
			_Force(initialForce)
		{

		}

		/*
		*	Equality operator overload.
		*/
		constexpr bool operator==(const InitializationData &other) const NOEXCEPT
		{
			return _Entity == other._Entity;
		}

	};

	/*
	*	Duplication data class definition.
	*/
	class DuplicationData final
	{

	public:

		//The entity to duplicate.
		const Entity *const RESTRICT _EntityToDuplicate;

		//The new entity.
		Entity *const RESTRICT _NewEntity;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE DuplicationData(const Entity *const RESTRICT initial_entity_to_duplicate, Entity *const RESTRICT initial_new_entity) NOEXCEPT
			:
			_EntityToDuplicate(initial_entity_to_duplicate),
			_NewEntity(initial_new_entity)
		{

		}

	};

	//The list of entities lock.
	Spinlock _EntitiesLock;

	//The list of entities.
	DynamicArray<Entity *RESTRICT> _Entities;

	//Lock for the initialization queue.
	Spinlock _InitializationQueueLock;

	//Container for all entities that have requested initialization.
	DynamicArray<InitializationData> _InitializationQueue;

	//Lock for the termination and destruction queue.
	Spinlock _TerminationDestructionQueueLock;

	//Container for all entities that have requested termination.
	DynamicArray<Entity *RESTRICT> _TerminationQueue;

	//Container for all entities that have requested destruction.
	DynamicArray<Entity *RESTRICT> _DestructionQueueOld;

	//Lock for the automatic termination queue.
	Spinlock _AutomaticTerminationQueueLock;

	//Container for all entities that have requested automatic termination.
	DynamicArray<Entity *RESTRICT> _AutomaticTerminationQueue;

	//Lock for the automatic destruction queue.
	Spinlock _AutomaticDestructionQueueLock;

	//Container for all entities that have requested automatic destruction.
	DynamicArray<Entity *RESTRICT> _AutomaticDestructionQueue;

	//Lock for the duplication queue.
	Spinlock _DuplicationQueueLock;

	//Container for all entities that have requested duplication.
	DynamicArray<DuplicationData> _DuplicationQueue;

	/*
	*	Processes the initialization queue.
	*/
	void ProcessInitializationQueue() NOEXCEPT;

	/*
	*	Processes the termination queue.
	*/
	void ProcessTerminationQueue() NOEXCEPT;

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueueOld() NOEXCEPT;

	/*
	*	Processes the automatic termination queue.
	*/
	void ProcessAutomaticTerminationQueue() NOEXCEPT;

	/*
	*	Processes the automatic destruction queue.
	*/
	void ProcessAutomaticDestructionQueue() NOEXCEPT;

	/*
	*	Processes the duplication queue.
	*/
	void ProcessDuplicationQueue() NOEXCEPT;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>