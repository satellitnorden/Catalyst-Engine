#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/Entity.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Forward declarations.
class EntityInitializationData;
class EntityTerminationData;
class EntityDestructionData;

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
	*	Updates the entity system synchronously during the closing update phase.
	*/
	void ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

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

	/*
	*	Requests the destruction of an entity.
	*	Destruction will happen at the next synchronous update of the entity system.
	*	Usually only one entity is destroyed at each update of the entity system.
	*	But if the destruction is forced, it will take priority and will be destroyed on the next update.
	*	So if N entities are forced for the next entity system update, all of them will be destroyed.
	*/
	void RequestDestruction(Entity *const RESTRICT entity, const bool force) NOEXCEPT;

	/*
	*	Initializes one entity.
	*/
	void InitializeEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates one entity.
	*/
	void TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT;

	/*
	*	Destroys one entity.
	*/
	void DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT;

private:

	/*
	*	Entity initialization data definition.
	*/
	class EntityInitializationData final
	{

	public:

		/*
		*	Default constructor, prohibited - must be constructed with the proper arguments.
		*/
		EntityInitializationData() NOEXCEPT = delete;

		/*
		*	Constructor taking all values as arguments.
		*/
		EntityInitializationData(Entity* const RESTRICT initialEntity, void* const RESTRICT initialData, const bool initialForce) NOEXCEPT
			:
			_Entity(initialEntity),
			_Data(initialData),
			_Force(initialForce)
		{

		}

		//The entity to initialize.
		Entity* RESTRICT _Entity;

		//The initialization data.
		void* RESTRICT _Data;

		//Denotes whether or not to force this initialization.
		bool _Force;

	};

	/*
	*	Entity termination data definition.
	*/
	class EntityTerminationData final
	{

	public:

		/*
		*	Default constructor, prohibited - must be constructed with the proper arguments.
		*/
		EntityTerminationData() NOEXCEPT = delete;

		/*
		*	Constructor taking all values as arguments.
		*/
		EntityTerminationData(Entity* const RESTRICT initialEntity, const bool initialForce) NOEXCEPT
			:
			_Entity(initialEntity),
			_Force(initialForce)
		{

		}

		//The entity to terminate.
		Entity* RESTRICT _Entity;

		//Denotes whether or not to force this termination.
		bool _Force;

	};

	/*
	*	Entity destruction data definition.
	*/
	class EntityDestructionData final
	{

	public:

		/*
		*	Default constructor, prohibited - must be constructed with the proper arguments.
		*/
		EntityDestructionData() NOEXCEPT = delete;

		/*
		*	Constructor taking all values as arguments.
		*/
		EntityDestructionData(Entity* const RESTRICT initialEntity, const bool initialForce) NOEXCEPT
			:
			_Entity(initialEntity),
			_Force(initialForce)
		{

		}

		//The entity to destroy.
		Entity* RESTRICT _Entity;

		//Denotes whether or not to force this destruction.
		bool _Force;

	};

	//The list of entities.
	DynamicArray<Entity *RESTRICT> _Entities;

	//Lock for the initialization queue.
	Spinlock _InitializationQueueLock;

	//Container for all entities that have requested initialization.
	DynamicArray<EntityInitializationData> _InitializationQueue;

	//Lock for the termination queue.
	Spinlock _TerminationQueueLock;

	//Container for all entities that have requested termination.
	DynamicArray<EntityTerminationData> _TerminationQueue;

	//Lock for the destruction queue.
	Spinlock _DestructionQueueLock;

	//Container for all entities that have requested destruction.
	DynamicArray<EntityDestructionData> _DestructionQueue;

	/*
	*	Processes the initialization queue.
	*/
	void ProcessInitializationQueue() NOEXCEPT;

	/*
	*	Initializes a dynamic physical entity.
	*/
	void InitializeDynamicPhysicalEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes a particle system physical entity.
	*/
	void InitializeParticleSystemEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(Entity* const RESTRICT entity, void* const RESTRICT data) NOEXCEPT;

	/*
	*	Processes the termination queue.
	*/
	void ProcessTerminationQueue() NOEXCEPT;

	/*
	*	Terminates a dynamic physical entity.
	*/
	void TerminateDynamicPhysicalEntity(Entity* const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates a terrain entity.
	*/
	void TerminateTerrainEntity(Entity* const RESTRICT entity) NOEXCEPT;

	/*
	*	Destroys initialization data for an entity.
	*/
	template <typename Type>
	void DestroyInitializationData(void* const RESTRICT data) NOEXCEPT;

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>