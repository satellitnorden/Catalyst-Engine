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
	*	Requests the initialization of en entity.
	*	Usually one entity is initialized at each update of the entity system.
	*	But if the initialization is forced, it will take priority and will be initialized on the next update.
	*/
	void RequestInitialization(Entity* const RESTRICT entity, void* const RESTRICT data, const bool force) NOEXCEPT;

	/*
	*	Creates initialization data for an entity.
	*/
	template <class InitializationDataClass>
	RESTRICTED InitializationDataClass* const RESTRICT CreateInitializationData() NOEXCEPT;

	/*
	*	Destroys initialization data for an entity.
	*/
	template <class InitializationDataClass>
	void DestroyInitializationData(void* const RESTRICT data) NOEXCEPT;

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

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>