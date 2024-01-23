//Header file.
#include <Systems/EntitySystem.h>

//Core.
#include <Core/General/Time.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

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