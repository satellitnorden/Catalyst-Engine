#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Forward declarations.
class Entity;
class EntityInitializationData;

class EntitySystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntitySystem);

	/*
	*	Default constructor.
	*/
	EntitySystem() NOEXCEPT = default;

	/*
	*	Pre-updates the entity system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the entity system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Requests the initialization of en entity.
	*	Initialization will happen at the next synchronous update of the entity system.
	*	Usually only one entity is initialized at each update of the entity system.
	*	But if the initialization is forced, it will take priority and will be initialized on the next update.
	*	So if N entities are forced for the next eneity system update, all of them will be initialized.
	*/
	void RequestInitialization(Entity* const RESTRICT entity, void* const RESTRICT data, const bool force) NOEXCEPT;

	/*
	*	Creates a new entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* const RESTRICT CreateEntity(Arguments&&... arguments) const NOEXCEPT;

	/*
	*	Creates a new child entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* const RESTRICT CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) const NOEXCEPT;

	/*
	*	Creates initialization data for an entity.
	*	Returns a pointer to the newly created data.
	*	The memory is freed after the entity system has finished initialization.
	*/
	template <class InitializationDataClass>
	RESTRICTED InitializationDataClass* const RESTRICT CreateInitializationData() NOEXCEPT;

private:

	//Lock for the initialization queue.
	Spinlock initializationQueueLock;

	//Container for all entities that have requested initialization.
	DynamicArray<EntityInitializationData> initializationQueue;

	/*
	*	Initializes entities.
	*/
	void InitializeEntities() NOEXCEPT;

	/*
	*	Initializes one entity.
	*/
	void InitializeEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(EntityInitializationData* const RESTRICT data) NOEXCEPT;

	/*
	*	Destroys initialization data for an entity.
	*/
	template <class InitializationDataClass>
	void DestroyInitializationData(void* const RESTRICT data) NOEXCEPT;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>