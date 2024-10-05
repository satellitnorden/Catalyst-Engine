//Header file.
#include <Systems/EntitySystem.h>

//Core.
#include <Core/General/Time.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>

//Entity system constants.
namespace EntitySystemConstants
{
	/*
	*	In the editor, just spawn everything as possible.
	*/
#if defined(CATALYST_EDITOR)
	constexpr float32 MAXIMUM_CREATION_TIME{ FLOAT32_MAXIMUM };
	constexpr float32 MAXIMUM_DESTRUCTION_TIME{ FLOAT32_MAXIMUM };
#else
	constexpr float32 MAXIMUM_CREATION_TIME{ 0.25f / 1000.0f };
	constexpr float32 MAXIMUM_DESTRUCTION_TIME{ 1.0f / 1000.0f };
#endif
}

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
*	Creates an entity.
*/
NO_DISCARD Entity *const RESTRICT EntitySystem::CreateEntity(const WorldTransform &world_transform, AssetPointer<EntityAsset> entity) NOEXCEPT
{
	//Just let entity serialization take care of this.
	uint64 stream_archive_position{ 0 };

	return EntitySerialization::DeserializeFromStreamArchive(entity->_StreamArchive, &stream_archive_position, &world_transform);
}

/*
*	Creates a new entity.
*/
NO_DISCARD Entity *const RESTRICT EntitySystem::CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT> component_configurations) NOEXCEPT
{
	while (_NumberOfItemsInCreationQueue.load() > 1024)
	{
		Concurrency::CurrentThread::Yield();
	}

	//Allocate the entity.
	Entity *RESTRICT entity;
	
	{
		SCOPED_LOCK(_EntityAllocatorLock);

		entity = static_cast<Entity *RESTRICT>(_EntityAllocator.Allocate());
		_Entities.Emplace(entity);
	}

	//Generate the entity identifier.
	entity->_EntityIdentifier = GenerateEntityIdentifier();

	//Reset whether or not this entity is initialized.
	CLEAR_BIT(entity->_Flags, Entity::Flags::INITIALIZED);

	//Add it to the creation queue.
	EntityCreationQueueItem queue_item;

	queue_item._Entity = entity;
	
	//Create a form of linked list for the component configurations.
	queue_item._ComponentConfigurations = component_configurations[0];
	queue_item._ComponentConfigurations->_NextComponentInitializationData = nullptr;

	{
		ComponentInitializationData *RESTRICT current_component_configuration{ queue_item._ComponentConfigurations };

		for (uint64 i{ 1 }; i < component_configurations.Size(); ++i)
		{
			current_component_configuration->_NextComponentInitializationData = component_configurations[i];
			current_component_configuration = component_configurations[i];
			current_component_configuration->_NextComponentInitializationData = nullptr;
		}
	}

	_CreationQueue.Push(queue_item);

	//Increment the number of items in the creation queue.
	++_NumberOfItemsInCreationQueue;

	//Return the entity.
	return entity;
}

/*
*	Adds a component to the given entity.
*/
void EntitySystem::AddComponentToEntity(Entity *const RESTRICT entity, ComponentInitializationData *RESTRICT component_configuration) NOEXCEPT
{
	//Add it to the creation queue.
	EntityCreationQueueItem queue_item;

	queue_item._Entity = entity;

	//Create a form of linked list for the component configurations.
	queue_item._ComponentConfigurations = component_configuration;
	queue_item._ComponentConfigurations->_NextComponentInitializationData = nullptr;

	_CreationQueue.Push(queue_item);

	//Increment the number of items in the creation queue.
	++_NumberOfItemsInCreationQueue;
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
*	Destroys all entities.
*/
void EntitySystem::DestroyAllEntities() NOEXCEPT
{
	SCOPED_LOCK(_EntityAllocatorLock);

	//Add them all to the queue.
	for (Entity *const RESTRICT entity : _Entities)
	{
		DestroyEntity(entity);
	}
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

	//Update the entity links.
	_EntityLinks.Update();
}

/*
*	Processes the creation queue.
*/
void EntitySystem::ProcessCreationQueue() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time;
	
	//Check the pre-processing queue.
	for (uint64 i{ 0 }; i < _PreProcessingQueue.Size();)
	{
		//Cache the queue item.
		EntityPreProcessingQueueItem *const RESTRICT queue_item{ _PreProcessingQueue[i] };

		//Check if pre-processing has finished.
		if (queue_item->_Task.IsExecuted())
		{
			//Notify all components that an instance is about to be created.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations };

				do
				{
					component_configuration->_Component->PreCreateInstance(queue_item->_CreationQueueItem._Entity);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}
			
			//Create all instances.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations };

				do
				{
					Components::CreateInstance(component_configuration->_Component, queue_item->_CreationQueueItem._Entity, component_configuration);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}

			//Notify all components that all instances has been created.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations };

				do
				{
					Components::PostCreateInstance(component_configuration->_Component, queue_item->_CreationQueueItem._Entity);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}

			//Free all initialization data.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_CreationQueueItem._ComponentConfigurations };

				do
				{
					ComponentInitializationData *RESTRICT next_component_configuration{ component_configuration->_NextComponentInitializationData };
					Components::FreeInitializationData(component_configuration->_Component, component_configuration);
					component_configuration = next_component_configuration;
				} while (component_configuration);
			}

			//This entity is now initialized. (:
			SET_BIT(queue_item->_CreationQueueItem._Entity->_Flags, Entity::Flags::INITIALIZED);

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
		//Decrement the number of items in the creation queue.
		--_NumberOfItemsInCreationQueue;

		//Check if any component needs pre-processing.
		bool any_component_needs_pre_processing{ false };

		//Notify all components that an instance is about to be created.
		{
			ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

			do
			{
				any_component_needs_pre_processing = Components::ShouldPreProcess(component_configuration->_Component);
				component_configuration = component_configuration->_NextComponentInitializationData;
			} while (component_configuration && !any_component_needs_pre_processing);
		}

		//If any component needs pre-processing, launch a task for it.
		if (any_component_needs_pre_processing)
		{
			//Set up the queue item.
			EntityPreProcessingQueueItem *const RESTRICT pre_processing_queue_item{ static_cast<EntityPreProcessingQueueItem *const RESTRICT>(_PreProcessingAllocator.Allocate()) };

			pre_processing_queue_item->_CreationQueueItem = *queue_item;
			pre_processing_queue_item->_Task._Function = [](void *const RESTRICT arguments)
			{
				EntityCreationQueueItem *const RESTRICT queue_item{ static_cast<EntityCreationQueueItem *const RESTRICT>(arguments) };

				{
					ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

					do
					{
						if (Components::ShouldPreProcess(component_configuration->_Component))
						{
							Components::PreProcess(component_configuration->_Component, component_configuration);
						}
						component_configuration = component_configuration->_NextComponentInitializationData;
					} while (component_configuration);
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
			{
				ComponentInitializationData* RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

				do
				{
					component_configuration->_Component->PreCreateInstance(queue_item->_Entity);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}

			//Create all instances.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

				do
				{
					Components::CreateInstance(component_configuration->_Component, queue_item->_Entity, component_configuration);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}

			//Notify all components that all instances has been created.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

				do
				{
					Components::PostCreateInstance(component_configuration->_Component, queue_item->_Entity);
					component_configuration = component_configuration->_NextComponentInitializationData;
				} while (component_configuration);
			}

			//Free all initialization data.
			{
				ComponentInitializationData *RESTRICT component_configuration{ queue_item->_ComponentConfigurations };

				do
				{
					ComponentInitializationData *RESTRICT next_component_configuration{ component_configuration->_NextComponentInitializationData };
					Components::FreeInitializationData(component_configuration->_Component, component_configuration);
					component_configuration = next_component_configuration;
				} while (component_configuration);
			}

			//This entity is now initialized. (:
			SET_BIT(queue_item->_Entity->_Flags, Entity::Flags::INITIALIZED);
		}

		//Break if we've run over time.
		if (start_time.GetSecondsSince() >= EntitySystemConstants::MAXIMUM_CREATION_TIME)
		{
			break;
		}
	}
}

/*
*	Processes the destruction queue.
*/
void EntitySystem::ProcessDestructionQueue() NOEXCEPT
{
	//Remember the "looparound" entity.
	EntityIdentifier looparound_entity{ UINT64_MAXIMUM };

	const TimePoint start_time;

	while (start_time.GetSecondsSince() < EntitySystemConstants::MAXIMUM_DESTRUCTION_TIME)
	{
		if (EntityDestructionQueueItem *const RESTRICT queue_item{ _DestructionQueue.Pop() })
		{
			//Check if we have looped around.
			if (looparound_entity != UINT64_MAXIMUM && queue_item->_Entity->_EntityIdentifier == looparound_entity)
			{
				_DestructionQueue.Push(*queue_item);

				return;
			}

			/*
			*	If the entity isn't initialized yet, we have to assume it's somewhere in the creation queue.
			*	Best we can do right now is just put it back into the queue at the end, and destroy it once initialized.
			*	Set the "looparound" entity here, if it's not set.
			*/
			if (!TEST_BIT(queue_item->_Entity->_Flags, Entity::Flags::INITIALIZED))
			{
				if (looparound_entity == UINT64_MAXIMUM)
				{
					looparound_entity = queue_item->_Entity->_EntityIdentifier;
				}

				_DestructionQueue.Push(*queue_item);

				continue;
			}

			//Notify all components that this entity was destroyed.
			for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
			{
				//Cache the component.
				Component *const RESTRICT component{ Components::At(component_index) };

				if (component->Has(queue_item->_Entity))
				{
					component->DestroyInstance(queue_item->_Entity);
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
				_Entities.Erase<false>(queue_item->_Entity);
			}

			//Remove the links for this entity as well.
			if (_EntityLinks.HasLinks(queue_item->_Entity))
			{
				_EntityLinks.RemoveLinks(queue_item->_Entity);
			}
		}

		else
		{
			break;
		}
	}
}