//Header file.
#include <Systems/EntitySystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Initializes the entity system.
*/
void EntitySystem::InitializeSystem() NOEXCEPT
{
	//Initialize the procedural placement update task.
	_ProceduralPlacementUpdateTask._Function = [](void *const RESTRICT)
	{
		EntitySystem::Instance->UpdateProceduralPlacement();
	};
	_ProceduralPlacementUpdateTask._Arguments = nullptr;
}

/*
*	Updates the entity system sequentially.
*/
void EntitySystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//If the procedural placement update has finished, cache the relevant data and fire off another update.
	if (_ProceduralPlacementUpdateTask.IsExecuted())
	{
		//Fire off another update.
		TaskSystem::Instance->ExecuteTask(&_ProceduralPlacementUpdateTask);
	}
}

/*
*	Updates the entity system synchronously during the closing update phase.
*/
void EntitySystem::ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Process the initialization queue.
	ProcessInitializationQueue();

	//Process the termination queue.
	ProcessTerminationQueue();

	//Process the destruction queue.
	ProcessDestructionQueue();

	//Process the automatic termination queue.
	ProcessAutomaticTerminationQueue();

	//Process the automatic destruction queue.
	ProcessAutomaticDestructionQueue();
}

/*
*	Initializes one entity.
*/
void EntitySystem::InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticDestruction))
	{
		ScopedLock<Spinlock> scopedLock{ _AutomaticDestructionQueueLock };

		_AutomaticDestructionQueue.EmplaceSlow(entity);
	}

	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticTermination))
	{
		ScopedLock<Spinlock> scopedLock{ _AutomaticTerminationQueueLock };

		_AutomaticTerminationQueue.EmplaceSlow(entity);
	}

	//Initialize this entity in different ways depending on the entity type.
	switch (entity->_Type)
	{
		case Entity::EntityType::DynamicPhysical:
		{
			InitializeDynamicPhysicalEntity(entity, data);

			break;
		}

		case Entity::EntityType::ParticleSystem:
		{
			InitializeParticleSystemEntity(entity, data);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			ASSERT(false, "Invalid entity type!");
		}
#endif
	}

	//Set this entity to initialized.
	entity->_Initialized = true;
}

/*
*	Terminates one entity.
*/
void EntitySystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	switch (entity->_Type)
	{
		case Entity::EntityType::DynamicPhysical:
		{
			TerminateDynamicPhysicalEntity(entity);

			break;
		}

		case Entity::EntityType::ParticleSystem:
		{
			TerminateParticleSystemEntity(entity);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			ASSERT(false, "Invalid entity type!");
		}
#endif
	}

	//Set this entity to un-initialized.
	entity->_Initialized = false;
}

/*
*	Destroys one entity.
*/
void EntitySystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	_Allocator.DeAllocate(entity);

	//Remove the entity from the list of entities.
	_Entities.Erase(entity);
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
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//Add the data.
	_InitializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Add the data.
	_TerminationQueue.EmplaceSlow(entity);
}

/*
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*/
void EntitySystem::RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add the data.
	_DestructionQueue.EmplaceSlow(entity);
}

/*
*	Registers a procedural placement function.
*/
void EntitySystem::RegisterProceduralPlacementFunction(EntityProceduralPlacementFunction function, const float gridSize) NOEXCEPT
{
	//Create the new procedural placement data.
	_ProceduralPlacementData.EmplaceSlow();
	EntityProceduralPlacementData *const RESTRICT data{ &_ProceduralPlacementData.Back() };

	//Fill in the details.
	data->_Function = function;
	data->_GridSize = gridSize;

	//Reset all grid points.
	for (EntityProceduralPlacementGridPoint &gridPoint : data->_GridPoints)
	{
		gridPoint._Valid = false;
	}
}

/*
*	Processes the initialization queue.
*/
void EntitySystem::ProcessInitializationQueue() NOEXCEPT
{
	//Lock the initialization queue.
	ScopedLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//If there's none to initialize, initialize none.
	if (_InitializationQueue.Empty())
	{
		return;
	}

	//Iterate through all initialization request and check for the force flag.
	uint64 forceInitialized{ 0 };
	uint64 counter{ _InitializationQueue.Size() - 1 };

	for (uint64 i = 0, size = _InitializationQueue.Size(); i < size; ++i)
	{
		InitializationData& data{ _InitializationQueue[counter] };

		if (data._Force)
		{
			InitializeEntity(data._Entity, data._Data);

			++forceInitialized;

			_InitializationQueue.EraseAt(counter);
		}

		--counter;
	}

	//If none were force-initialized, just initialize one.
	if (forceInitialized == 0)
	{
		InitializationData *const RESTRICT data = &_InitializationQueue.Back();
		InitializeEntity(data->_Entity, data->_Data);
		_InitializationQueue.PopFast();
	}
}

/*
*	Initializes a dynamic physical entity.
*/
void EntitySystem::InitializeDynamicPhysicalEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this dynamic physical entity.
	entity->_ComponentsIndex = ComponentManager::GetNewDynamicPhysicalComponentsIndex(entity);

	//Initialize the dynamic physical entity via the rendering system.
	RenderingSystem::Instance->InitializeDynamicPhysicalEntity(entity, static_cast<const DynamicPhysicalInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<DynamicPhysicalInitializationData>(data);
}

/*
*	Initializes a particle system physical entity.
*/
void EntitySystem::InitializeParticleSystemEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this particle system entity.
	entity->_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(entity);

	//Initialize the particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(entity, static_cast<const ParticleSystemInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Processes the termination queue.
*/
void EntitySystem::ProcessTerminationQueue() NOEXCEPT
{
	//Lock the termination queue.
	ScopedLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Terminate all entities.
	for (Entity *const RESTRICT entity : _TerminationQueue)
	{
		TerminateEntity(entity);
	}

	//Clear the termination queue.
	_TerminationQueue.ClearFast();
}

/*
*	Terminates a dynamic physical entity.
*/
void EntitySystem::TerminateDynamicPhysicalEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicPhysicalComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a particle system entity.
*/
void EntitySystem::TerminateParticleSystemEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate the particle system entity via the rendering system.
	RenderingSystem::Instance->TerminateParticleSystemEntity(entity);

	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Processes the destruction queue.
*/
void EntitySystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Destroy all entities.
	for (Entity *const RESTRICT entity : _DestructionQueue)
	{
		DestroyEntity(entity);
	}

	//Clear the destruction queue.
	_DestructionQueue.ClearFast();
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

			_AutomaticTerminationQueue.EraseAt(i);
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
			DestroyEntity(entity);

			_AutomaticDestructionQueue.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Updates the procedural placement.
*/
void EntitySystem::UpdateProceduralPlacement() NOEXCEPT
{
	//Cache the current viewer position.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

	//Update all procedural placement data.
	for (EntityProceduralPlacementData &data : _ProceduralPlacementData)
	{
		//Calculate the current grid point based on the current viewer position.
		const GridPoint3 currentGridPoint{ GridPoint3::WorldPositionToGridPoint(viewerPosition, data._GridSize) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint3, 27> validGridPoints
		{
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z + 1),

			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z + 1),

			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z + 1),
		};

		//Determine the grid points to invalidate.
		for (EntityProceduralPlacementGridPoint &currentGridPoint : data._GridPoints)
		{
			//If this grid point is already invalid, no need to invalidate it.
			if (!currentGridPoint._Valid)
			{
				continue;
			}

			bool valid{ false };

			for (const GridPoint3 &validGridPoint : validGridPoints)
			{
				if (currentGridPoint._GridPoint == validGridPoint)
				{
					valid = true;

					break;
				}
			}

			if (!valid)
			{
				//Invalidate this grid point.
				currentGridPoint._Valid = false;

				for (Entity *const RESTRICT entity : currentGridPoint._Entities)
				{
					RequestTermination(entity);
					RequestDestruction(entity);
				}

				currentGridPoint._Entities.ClearFast();
			}
		}

		//Determine the grid points to update.
		for (const GridPoint3 &validGridPoint : validGridPoints)
		{
			bool exists{ false };

			for (EntityProceduralPlacementGridPoint &currentGridPoint : data._GridPoints)
			{
				if (currentGridPoint._Valid && currentGridPoint._GridPoint == validGridPoint)
				{
					exists = true;

					break;
				}
			}

			if (!exists)
			{
				//Find the first invalid grid point and update it.
				for (EntityProceduralPlacementGridPoint &currentGridPoint : data._GridPoints)
				{
					if (!currentGridPoint._Valid)
					{
						currentGridPoint._Valid = true;
						currentGridPoint._GridPoint = validGridPoint;

						//Construct the axis aligned bounding box.
						const Vector3 currentGridPointWorldPosition{ GridPoint3::GridPointToWorldPosition(currentGridPoint._GridPoint, data._GridSize) };

						AxisAlignedBoundingBox box;

						box._Minimum._X = currentGridPointWorldPosition._X - (data._GridSize * 0.5f);
						box._Minimum._Y = currentGridPointWorldPosition._Y - (data._GridSize * 0.5f);
						box._Minimum._Z = currentGridPointWorldPosition._Z - (data._GridSize * 0.5f);

						box._Maximum._X = currentGridPointWorldPosition._X - (data._GridSize * 0.5f);
						box._Maximum._Y = currentGridPointWorldPosition._Y - (data._GridSize * 0.5f);
						box._Maximum._Z = currentGridPointWorldPosition._Z - (data._GridSize * 0.5f);

						data._Function(box, &currentGridPoint._Entities);

						break;
					}
				}
			}
		}
	}
}