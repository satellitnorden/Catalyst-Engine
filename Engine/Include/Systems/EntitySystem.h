#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>
#include <Entities/Types/Entity.h>

class EntitySystem final
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
	*	Updates the entity system during the post update phase.
	*/
	void PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Creates a new entity.
	*/
	template <class CLASS, class... ARGUMENTS>
	RESTRICTED CLASS* const RESTRICT CreateEntity(ARGUMENTS&&... arguments) NOEXCEPT;

	/*
	*	Creates initialization data for an entity.
	*/
	template <typename TYPE>
	RESTRICTED TYPE* const RESTRICT CreateInitializationData() NOEXCEPT;

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
	void DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT;

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

private:

	/*
	*	Initialization data definition.
	*/
	class InitializationData final
	{

	public:

		//The entity to initialize.
		Entity* RESTRICT _Entity;

		//The initialization data.
		EntityInitializationData* RESTRICT _Data;

		//Denotes whether or not to force this initialization.
		bool _Force;

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

	//The allocator lock.
	Spinlock _AllocatorLock;

	//The allocator.
	PoolAllocator<sizeof(Entity)> _Allocator;

	//The list of entities lock.
	Spinlock _EntitiesLock;

	//The list of entities.
	DynamicArray<Entity *RESTRICT> _Entities;

	//Lock for the initialization queue.
	Spinlock _InitializationQueueLock;

	//Container for all entities that have requested initialization.
	DynamicArray<InitializationData> _InitializationQueue;

	//Lock for the termination queue.
	Spinlock _TerminationQueueLock;

	//Container for all entities that have requested termination.
	DynamicArray<Entity *RESTRICT> _TerminationQueue;

	//Lock for the destruction queue.
	Spinlock _DestructionQueueLock;

	//Container for all entities that have requested destruction.
	DynamicArray<Entity *RESTRICT> _DestructionQueue;

	//Lock for the automatic termination queue.
	Spinlock _AutomaticTerminationQueueLock;

	//Container for all entities that have requested automatic termination.
	DynamicArray<Entity *RESTRICT> _AutomaticTerminationQueue;

	//Lock for the automatic destruction queue.
	Spinlock _AutomaticDestructionQueueLock;

	//Container for all entities that have requested automatic destruction.
	DynamicArray<Entity *RESTRICT> _AutomaticDestructionQueue;

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
	void ProcessDestructionQueue() NOEXCEPT;

	/*
	*	Processes the automatic termination queue.
	*/
	void ProcessAutomaticTerminationQueue() NOEXCEPT;

	/*
	*	Processes the automatic destruction queue.
	*/
	void ProcessAutomaticDestructionQueue() NOEXCEPT;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>