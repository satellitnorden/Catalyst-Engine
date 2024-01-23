//Header file.
#include <Systems/EntitySystem.h>

//Core.
#include <Core/General/Time.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Entities.
#include <Entities/Types/ParticleSystemEntity.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>

//Entity system constants.
namespace EntitySystemConstants
{
	constexpr float32 MAXIMUM_CREATION_TIME{ 0.25f / 1000.0f };
	constexpr float32 MAXIMUM_DESTRUCTION_TIME{ 0.5f / 1000.0f };
}

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Initializes the entity system.
*/
void EntitySystem::Initialize() NOEXCEPT
{
	//Reserve an appropriate size for the various containers.
	_EntitiesLock.Lock();
	_Entities.Reserve(1'024);
	_EntitiesLock.Unlock();

	_InitializationQueueLock.Lock();
	_InitializationQueue.Reserve(1'024);
	_InitializationQueueLock.Unlock();

	_TerminationDestructionQueueLock.Lock();
	_TerminationQueue.Reserve(1'024);
	_DestructionQueueOld.Reserve(1'024);
	_TerminationDestructionQueueLock.Unlock();

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<EntitySystem *const RESTRICT>(arguments)->EntityUpdate();
	},
	this,
	UpdatePhase::ENTITY,
	UpdatePhase::INPUT,
	false,
	false);
}

/*
*	Creates a new entity.
*/
NO_DISCARD Entity *const RESTRICT EntitySystem::CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT> component_configurations) NOEXCEPT
{
	//Allocate the entity.
	Entity *RESTRICT entity;
	
	{
		SCOPED_LOCK(_EntityAllocatorLock);

		entity = static_cast<Entity *RESTRICT>(_EntityAllocator.Allocate());
	}

	//Generate the entity identifier.
	entity->_EntityIdentifier = GenerateEntityIdentifier();

	//Reset whether or not this entity is initialized.
	entity->_Initialized = false;

	//Add it to the creation queue.
	EntityCreationQueueItem queue_item;

	queue_item._Entity = entity;
	
	queue_item._NumberOfComponentConfigurations = 0;

	for (ComponentInitializationData *const RESTRICT &component_configuration : component_configurations)
	{
		queue_item._ComponentConfigurations[queue_item._NumberOfComponentConfigurations] = component_configuration;

		++queue_item._NumberOfComponentConfigurations;
	}

	_CreationQueue.Push(queue_item);

	//Return the entity.
	return entity;
}

/*
*	Destroys an entity.
*/
void EntitySystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//Add it to the destruction queue.
	EntityDestructionQueueItem queue_item;

	queue_item._Entity = entity;

	_DestructionQueue.Push(queue_item);
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AUTOMATIC_TERMINATION))
	{
		SCOPED_LOCK(_AutomaticTerminationQueueLock);

		_AutomaticTerminationQueue.Emplace(entity);
	}

	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (TEST_BIT(data->_Properties, EntityInitializationData::Property::AUTOMATIC_DESTRUCTION))
	{
		SCOPED_LOCK(_AutomaticDestructionQueueLock);

		_AutomaticDestructionQueue.Emplace(entity);
	}

	//Initialize this entity.
	entity->Initialize(data);

	//Set this entity to initialized.
	entity->_Initialized = true;
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate this entity.
	entity->Terminate();

	//Set this entity to un-initialized.
	entity->_Initialized = false;
}

/*
*	Destroys one entity.
*/
void EntitySystem::DestroyEntityOld(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(!entity->_Initialized, "Destroying an initialized entity, this is bad!");

	//The entity should already be terminated, so just deallocate the entity.
	_EntityAllocatorLock.Lock();
	_EntityAllocator.Free(entity);
	_EntityAllocatorLock.Unlock();

	//Remove the entity from the list of entities.
	_EntitiesLock.Lock();
	_Entities.Erase<false>(entity);
	_EntitiesLock.Unlock();
}

/*
*	Requests the initialization of en entity.
*	Initialization will happen at the next synchronous update of the entity system.
*	Usually only one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*	So if N entities are forced for the next entity system update, all of them will be initialized.
*/
void EntitySystem::RequestInitialization(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data, const bool force) NOEXCEPT
{
	ASSERT(!entity->_Initialized, "Don't call EntityCreationSystem::RequestInitialization() on entities that already are initialized!");

	//Lock the queue.
	SCOPED_LOCK(_InitializationQueueLock);

	//Add the data.
	_InitializationQueue.Emplace(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	{
		//Lock the queue.
		SCOPED_LOCK(_TerminationDestructionQueueLock);

		//Add the data.
		_TerminationQueue.Emplace(entity);
	}
}

/*
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	SCOPED_LOCK(_TerminationDestructionQueueLock);

	//Add the data.
	_DestructionQueueOld.Emplace(entity);
}

/*
*	Requests the duplication of the given entity.
*	Initialization will happen at the next synchronous update of the entity system.
*/
RESTRICTED NO_DISCARD Entity *const RESTRICT EntitySystem::DuplicateEntity(const Entity *const RESTRICT entity) NOEXCEPT
{
	//Create the new entity.
	Entity *RESTRICT new_entity;

	switch (entity->_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:			\
		{													\
			new_entity = CreateEntity<VALUE ## Entity>();	\
															\
			break;											\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}

	{
		//Lock the duplication queue.
		SCOPED_LOCK(_DuplicationQueueLock);

		//Add to the duplication queue.
		_DuplicationQueue.Emplace(entity, new_entity);
	}

	//Return the new entity.
	return new_entity;
}

/*
*	Terminates/destroys all entities.
*/
void EntitySystem::DestroyAllEntities() NOEXCEPT
{
	//Lock the entities.
	_EntitiesLock.Lock();
	
	//Create a copy of the entities list.
	DynamicArray<Entity *RESTRICT> entities{ _Entities };

	//Unlock the entities.
	_EntitiesLock.Unlock();

	//Terminate/destroy all entities.
	for (Entity *const RESTRICT entity : entities)
	{
		if (entity->_Initialized)
		{
			RequestTermination(entity);
		}

		RequestDestruction(entity);
	}

	//Lock the entities.
	_EntitiesLock.Lock();

	//Clear the entities.
	_Entities.Clear();

	//Unlock the entities.
	_EntitiesLock.Unlock();
}

/*
*	Generates a new entity identifier.
*/
NO_DISCARD EntityIdentifier EntitySystem::GenerateEntityIdentifier() NOEXCEPT
{
	SCOPED_LOCK(_EntityIdentifierLock);

	if (!_EntityIdentifierFreeList.Empty())
	{
		const EntityIdentifier new_entity_identifier{ _EntityIdentifierFreeList.Back() };
		_EntityIdentifierFreeList.Pop();
		return new_entity_identifier;
	}

	else
	{
		return _EntityIdentifierCounter++;
	}
}

/*
*	Updates the entity system during the ENTITY update phase.
*/
void EntitySystem::EntityUpdate() NOEXCEPT
{
	//Process the creation queue.
	ProcessCreationQueue();

	//Process the destruction queue.
	ProcessDestructionQueue();

	//Process the initialization queue.
	ProcessInitializationQueue();

	{
		SCOPED_LOCK(_TerminationDestructionQueueLock);

		//Process the termination queue.
		ProcessTerminationQueue();

		//Process the destruction queue.
		ProcessDestructionQueueOld();
	}

	//Process the automatic termination queue.
	ProcessAutomaticTerminationQueue();

	//Process the automatic destruction queue.
	ProcessAutomaticDestructionQueue();

	//Process the duplication queue.
	ProcessDuplicationQueue();
}

/*
*	Processed the creation queue.
*/
void EntitySystem::ProcessCreationQueue() NOEXCEPT
{
	//Cache the start time.
	TimePoint start_time{ GetCurrentTimePoint() };
	
	//Check the pre-processing queue.
	for (uint64 i{ 0 }; i < _PreProcessingQueue.Size();)
	{
		//Cache the queue item.
		EntityPreProcessingQueueItem *const RESTRICT queue_item{ _PreProcessingQueue[i] };

		//Check if pre-processing has finished.
		if (queue_item->_Task.IsExecuted())
		{
			//Notify all components that an instance is about to be created.
			for (uint64 i{ 0 }; i < queue_item->_CreationQueueItem._NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations[i] };
				component_configuration->_Component->PreCreateInstance(queue_item->_CreationQueueItem._Entity->_EntityIdentifier);
			}

			//Create all instances.
			for (uint64 i{ 0 }; i < queue_item->_CreationQueueItem._NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations[i] };
				component_configuration->_Component->CreateInstance(queue_item->_CreationQueueItem._Entity->_EntityIdentifier, component_configuration);
			}

			//Notify all components that all instances has been created.
			for (uint64 i{ 0 }; i < queue_item->_CreationQueueItem._NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations[i] };
				component_configuration->_Component->PostCreateInstance(queue_item->_CreationQueueItem._Entity->_EntityIdentifier);
			}

			//This entity is now initialized. (:
			queue_item->_CreationQueueItem._Entity->_Initialized = true;

			//Free the queue item.
			_PreProcessingAllocator.Free(queue_item);

			//Remove the queue item.
			_PreProcessingQueue.EraseAt<false>(i);
		}

		else
		{
			++i;
		}

		//Break if we've run over time.
		if (start_time.GetSecondsSince() >= EntitySystemConstants::MAXIMUM_CREATION_TIME)
		{
			break;
		}
	}

	//Check the creation queue.
	while (EntityCreationQueueItem *const RESTRICT queue_item{ _CreationQueue.Pop() })
	{
		//Check if any component needs pre-processing.
		bool any_componend_needs_pre_processing{ false };

		for (uint64 i{ 0 }; i < queue_item->_NumberOfComponentConfigurations; ++i)
		{
			if (queue_item->_ComponentConfigurations[i]->_Component->NeedsPreProcessing())
			{
				any_componend_needs_pre_processing = true;

				break;
			}
		}

		//If any component needs pre-processing, launch a task for it.
		if (any_componend_needs_pre_processing)
		{
			//Set up the queue item.
			EntityPreProcessingQueueItem *const RESTRICT pre_processing_queue_item{ static_cast<EntityPreProcessingQueueItem *const RESTRICT>(_PreProcessingAllocator.Allocate()) };

			pre_processing_queue_item->_CreationQueueItem = *queue_item;
			pre_processing_queue_item->_Task._Function = [](void *const RESTRICT arguments)
			{
				EntityCreationQueueItem *const RESTRICT queue_item{ static_cast<EntityCreationQueueItem *const RESTRICT>(arguments) };

				for (uint64 i{ 0 }; i < queue_item->_NumberOfComponentConfigurations; ++i)
				{
					if (queue_item->_ComponentConfigurations[i]->_Component->NeedsPreProcessing())
					{
						queue_item->_ComponentConfigurations[i]->_Component->PreProcess(queue_item->_ComponentConfigurations[i]);
					}
				}
			};
			pre_processing_queue_item->_Task._Arguments = &pre_processing_queue_item->_CreationQueueItem;
			pre_processing_queue_item->_Task._ExecutableOnSameThread = false;

			//Execute the task!
			TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &pre_processing_queue_item->_Task);

			//Add it to the queue.
			_PreProcessingQueue.Emplace(pre_processing_queue_item);
		}

		//Otherwise, create the entity as usual!
		else
		{
			//Notify all components that an instance is about to be created.
			for (uint64 i{ 0 }; i < queue_item->_NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations[i] };
				component_configuration->_Component->PreCreateInstance(queue_item->_Entity->_EntityIdentifier);
			}

			//Create all instances.
			for (uint64 i{ 0 }; i < queue_item->_NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations[i] };
				component_configuration->_Component->CreateInstance(queue_item->_Entity->_EntityIdentifier, component_configuration);
			}

			//Notify all components that all instances has been created.
			for (uint64 i{ 0 }; i < queue_item->_NumberOfComponentConfigurations; ++i)
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations[i] };
				component_configuration->_Component->PostCreateInstance(queue_item->_Entity->_EntityIdentifier);
			}

			//This entity is now initialized. (:
			queue_item->_Entity->_Initialized = true;
		}

		//Break if we've run over time.
		if (start_time.GetSecondsSince() >= EntitySystemConstants::MAXIMUM_CREATION_TIME)
		{
			break;
		}
	}
}

/*
*	Processed the destruction queue.
*/
void EntitySystem::ProcessDestructionQueue() NOEXCEPT
{
	TimePoint start_time{ GetCurrentTimePoint() };

	while (start_time.GetSecondsSince() < EntitySystemConstants::MAXIMUM_DESTRUCTION_TIME)
	{
		if (EntityDestructionQueueItem *const RESTRICT queue_item{ _DestructionQueue.Pop() })
		{
			/*
			*	If the entity isn't initialized yet, we have to assume it's somewhere in the creation queue.
			*	Best we can do right now is just put it back into the queue at the end, and destroy it once initialized.
			*	Return here to avoid being stuck in a loop the rest of the time-slice,
			*	if the entity in question takes a long time to be created.
			*/
			if (!queue_item->_Entity->_Initialized)
			{
				_DestructionQueue.Push(*queue_item);

				return;
			}

			//Notify all components that this entity was destroyed.
			for (Component *const RESTRICT component : AllComponents())
			{
				if (component->Has(queue_item->_Entity->_EntityIdentifier))
				{
					component->DestroyInstance(queue_item->_Entity->_EntityIdentifier);
				}
			}

			//Add this entity's identifier to the free list.
			{
				SCOPED_LOCK(_EntityIdentifierLock);

				_EntityIdentifierFreeList.Emplace(queue_item->_Entity->_EntityIdentifier);
			}

			//Free the memory for this entity.
			{
				SCOPED_LOCK(_EntityAllocatorLock);

				_EntityAllocator.Free(queue_item->_Entity);
			}
		}

		else
		{
			break;
		}
	}
}

/*
*	Processes the initialization queue.
*/
void EntitySystem::ProcessInitializationQueue() NOEXCEPT
{
	//Lock the initialization queue.
	SCOPED_LOCK(_InitializationQueueLock);

	//If there's none to initialize, initialize none.
	if (_InitializationQueue.Empty())
	{
		return;
	}

	//Iterate through all initialization request and check for the force flag.
	uint64 force_initialized{ 0 };

	for (int64 i{ static_cast<int64>(_InitializationQueue.LastIndex()) }; i >= 0; --i)
	{
		InitializationData &data{ _InitializationQueue[i] };

		if (data._Force)
		{
#if !defined(CATALYST_CONFIGURATION)
			{
				EntityPreprocessingParameters parameters;

				data._Entity->GetPreprocessingParameters(&parameters);

				ASSERT(!parameters._ShouldPreprocess, "Can't force entities that require preprocessing!");
			}
#endif

			InitializeEntity(data._Entity, data._Data);

			++force_initialized;

			_InitializationQueue.EraseAt<false>(i);
		}
	}

	//If none were force-initialized, just initialize one.
	for (uint64 i{ 0 }, size{ _InitializationQueue.Size() }; i < size; ++i)
	{
		InitializationData &data{ _InitializationQueue[i] };

		EntityPreprocessingParameters parameters;

		data._Entity->GetPreprocessingParameters(&parameters);

		if (parameters._ShouldPreprocess)
		{
			if (data._PreprocessingData)
			{
				if (data._PreprocessingData->_Task.IsExecuted())
				{
					InitializeEntity(data._Entity, data._Data);

					MemorySystem::Instance->TypeFree<InitializationData::PreprocessingData>(data._PreprocessingData);

					_InitializationQueue.EraseAt<false>(i);

					break;
				}

				else
				{
					continue;
				}
			}

			else
			{
				data._PreprocessingData = MemorySystem::Instance->TypeAllocate<InitializationData::PreprocessingData>();

				data._PreprocessingData->_Task._Function = [](void *const RESTRICT arguments)
				{
					InitializationData::PreprocessingData *const RESTRICT preprocessing_data{ static_cast<InitializationData::PreprocessingData*const RESTRICT>(arguments) };

					preprocessing_data->_Entity->Preprocess(preprocessing_data->_Data);
				};
				data._PreprocessingData->_Task._Arguments = data._PreprocessingData;
				data._PreprocessingData->_Task._ExecutableOnSameThread = parameters._CanPreprocessOnMainThread;
				data._PreprocessingData->_Entity = data._Entity;
				data._PreprocessingData->_Data = data._Data;

				TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &data._PreprocessingData->_Task);
			}
		}

		else
		{
			InitializeEntity(data._Entity, data._Data);

			_InitializationQueue.EraseAt<false>(i);

			break;
		}
	}
}

/*
*	Processes the termination queue.
*/
void EntitySystem::ProcessTerminationQueue() NOEXCEPT
{
	//Terminate all entities.
	for (uint64 i{ 0 }; i < _TerminationQueue.Size();)
	{
		if (_TerminationQueue[i]->_Initialized)
		{
			TerminateEntity(_TerminationQueue[i]);

			_TerminationQueue.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Processes the destruction queue.
*/
void EntitySystem::ProcessDestructionQueueOld() NOEXCEPT
{
	//Destroy all entities.
	for (uint64 i{ 0 }; i < _DestructionQueueOld.Size();)
	{
		if (!_DestructionQueueOld[i]->_Initialized
			&& !_TerminationQueue.Exists(_DestructionQueueOld[i]))
		{
			DestroyEntityOld(_DestructionQueueOld[i]);

			_DestructionQueueOld.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Processes the automatic termination queue.
*/
void EntitySystem::ProcessAutomaticTerminationQueue() NOEXCEPT
{
	for (uint64 i = 0; i < _AutomaticTerminationQueue.Size();)
	{
		Entity *const RESTRICT entity{ _AutomaticTerminationQueue[i] };

		if (entity->ShouldAutomaticallyTerminate())
		{
			TerminateEntity(entity);

			_AutomaticTerminationQueue.EraseAt<false>(i);
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
void EntitySystem::ProcessAutomaticDestructionQueue() NOEXCEPT
{
	for (uint64 i = 0; i < _AutomaticDestructionQueue.Size();)
	{
		Entity *const RESTRICT entity{ _AutomaticDestructionQueue[i] };

		if (!entity->_Initialized)
		{
			DestroyEntityOld(entity);

			_AutomaticDestructionQueue.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Processes the duplication queue.
*/
void EntitySystem::ProcessDuplicationQueue() NOEXCEPT
{
	//Process the duplication queue.
	for (DuplicationData &duplication_data : _DuplicationQueue)
	{
		//Retrieve the duplication initialization data.
		EntityInitializationData* const RESTRICT initialization_data{ duplication_data._EntityToDuplicate->GetDuplicationInitializationData() };

		//Request the initialization for the new entity.
		RequestInitialization(duplication_data._NewEntity, initialization_data, false);
	}

	//Clear the duplication queue.
	_DuplicationQueue.Clear();
}