#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/Entity.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Forward declarations.
class EntityInitializationData;
class EntityTerminationData;

class EntitySystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntitySystem);

	/*
	*	Default constructor.
	*/
	EntitySystem() NOEXCEPT;

	/*
	*	Pre-updates the entity system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the entity system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Creates a new entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* const RESTRICT CreateEntity(Arguments&&... arguments) NOEXCEPT;

	/*
	*	Creates a new child entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* const RESTRICT CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) NOEXCEPT;

	/*
	*	Creates initialization data for an entity.
	*	Returns a pointer to the newly created data.
	*	The memory is freed after the entity system has finished initialization.
	*/
	template <typename Type>
	RESTRICTED Type* const RESTRICT CreateInitializationData() NOEXCEPT;

	/*
	*	Requests the initialization of en entity.
	*	Initialization will happen at the next synchronous update of the entity system.
	*	Usually only one entity is initialized at each update of the entity system.
	*	But if the initialization is forced, it will take priority and will be initialized on the next update.
	*	So if N entities are forced for the next entity system update, all of them will be initialized.
	*/
	void RequestInitialization(Entity* const RESTRICT entity, void* const RESTRICT data, const bool force) NOEXCEPT;

	/*
	*	Requests the termination of en entity.
	*	Termination will happen at the next synchronous update of the entity system.
	*	Usually only one entity is terminated at each update of the entity system.
	*	But if the termination is forced, it will take priority and will be terminated on the next update.
	*	So if N entities are forced for the next entity system update, all of them will be terminated.
	*/
	void RequesTermination(Entity* const RESTRICT entity, const bool force) NOEXCEPT;

private:

	//The list of entities.
	DynamicArray<Entity *RESTRICT> entities;

	//Lock for the initialization queue.
	Spinlock initializationQueueLock;

	//Container for all entities that have requested initialization.
	DynamicArray<EntityInitializationData> initializationQueue;

	//Lock for the termination queue.
	Spinlock terminationQueueLock;

	//Container for all entities that have requested termination.
	DynamicArray<EntityTerminationData> terminationQueue;

	/*
	*	Initializes entities.
	*/
	void InitializeEntities() NOEXCEPT;

	/*
	*	Initializes one entity.
	*/
	void InitializeEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes a dynamic physical entity.
	*/
	void InitializeDynamicPhysicalEntity(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates entities.
	*/
	void TerminateEntities() NOEXCEPT;

	/*
	*	Terminates one entity.
	*/
	void TerminateEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates a terrain entity.
	*/
	void TerminateTerrainEntity(EntityTerminationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Destroys initialization data for an entity.
	*/
	template <typename Type>
	void DestroyInitializationData(void* const RESTRICT data) NOEXCEPT;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>