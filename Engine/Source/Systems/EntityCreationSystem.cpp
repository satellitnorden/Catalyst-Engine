//Header file.
#include <Systems/EntityCreationSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>
#include <Entities/Creation/ParticleSystemInitializationData.h>
#include <Entities/Creation/PhysicsInitializationData.h>
#include <Entities/Creation/PointLightInitializationData.h>
#include <Entities/Creation/SoundInitializationData.h>

//Multithreading.
#include <Multithreading/ScopedWriteLock.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntityCreationSystem);

/*
*	Post-updates the entity creation system synchronously.
*/
void EntityCreationSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
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
void EntityCreationSystem::InitializeEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//If this entity is intialized with the automatic destruction property, add it to the automatic destruction queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticDestruction))
	{
		ScopedWriteLock<Spinlock> scopedLock{ _AutomaticDestructionQueueLock };

		_AutomaticDestructionQueue.EmplaceSlow(entity);
	}

	//If this entity is intialized with the automatic termination property, add it to the automatic termination queue.
	if (IS_BIT_SET(data->_Properties, EntityInitializationData::EntityProperty::AutomaticTermination))
	{
		ScopedWriteLock<Spinlock> scopedLock{ _AutomaticTerminationQueueLock };

		_AutomaticTerminationQueue.EmplaceSlow(entity);
	}

	//Initialize this entity in different ways depending on the entity type.
	switch (entity->_Type)
	{
		case EntityType::DynamicPhysical:
		{
			InitializeDynamicPhysicalEntity(entity, data);

			break;
		}

		case EntityType::ParticleSystem:
		{
			InitializeParticleSystemEntity(entity, data);

			break;
		}

		case EntityType::Physics:
		{
			InitializePhysicsEntity(entity, data);

			break;
		}

		case EntityType::PointLight:
		{
			InitializePointLightEntity(entity, data);

			break;
		}

		case EntityType::Sound:
		{
			InitializeSoundEntity(entity, data);

			break;
		}

#if defined(CATALYST_CONFIGURATION_DEBUG)
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
void EntityCreationSystem::TerminateEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	switch (entity->_Type)
	{
		case EntityType::DynamicPhysical:
		{
			TerminateDynamicPhysicalEntity(entity);

			break;
		}

		case EntityType::ParticleSystem:
		{
			TerminateParticleSystemEntity(entity);

			break;
		}

		case EntityType::Physics:
		{
			TerminatePhysicsEntity(entity);

			break;
		}

		case EntityType::PointLight:
		{
			TerminatePointLightEntity(entity);

			break;
		}

		case EntityType::Sound:
		{
			TerminateSoundEntity(entity);

			break;
		}

#if defined(CATALYST_CONFIGURATION_DEBUG)
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
void EntityCreationSystem::DestroyEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//The entity should already be terminated, so just deallocate the entity.
	_AllocatorLock.WriteLock();
	_Allocator.DeAllocate(entity);
	_AllocatorLock.WriteUnlock();

	//Remove the entity from the list of entities.
	_EntitiesLock.WriteLock();
	_Entities.Erase(entity);
	_EntitiesLock.WriteUnlock();
}

/*
*	Requests the initialization of en entity.
*	Initialization will happen at the next synchronous update of the entity system.
*	Usually only one entity is initialized at each update of the entity system.
*	But if the initialization is forced, it will take priority and will be initialized on the next update.
*	So if N entities are forced for the next entity system update, all of them will be initialized.
*/
void EntityCreationSystem::RequestInitialization(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data, const bool force) NOEXCEPT
{
	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _InitializationQueueLock };

	//Add the data.
	_InitializationQueue.EmplaceSlow(entity, data, force);
}

/*
*	Requests the termination of en entity.
*	Termination will happen at the next synchronous update of the entity system.
*/
void EntityCreationSystem::RequestTermination(Entity* const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _TerminationQueueLock };

	//Add the data.
	_TerminationQueue.EmplaceSlow(entity);
}

/*
*	Requests the destruction of an entity.
*	Destruction will happen at the next synchronous update of the entity system.
*/
void EntityCreationSystem::RequestDestruction(Entity *const RESTRICT entity) NOEXCEPT
{
	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add the data.
	_DestructionQueue.EmplaceSlow(entity);
}

/*
*	Processes the initialization queue.
*/
void EntityCreationSystem::ProcessInitializationQueue() NOEXCEPT
{
	//Lock the initialization queue.
	ScopedWriteLock<Spinlock> scopedLock{ _InitializationQueueLock };

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
void EntityCreationSystem::InitializeDynamicPhysicalEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
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
void EntityCreationSystem::InitializeParticleSystemEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this particle system entity.
	entity->_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(entity);

	//Initialize the particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(entity, static_cast<const ParticleSystemInitializationData *const RESTRICT>(data));

	//Destroy the initialization data.
	DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Initializes a physics entity.
*/
void EntityCreationSystem::InitializePhysicsEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	entity->_ComponentsIndex = ComponentManager::GetNewPhysicsComponentsIndex(entity);

	//Copy the data over to the component.
	PhysicsComponent &physicsComponent{ ComponentManager::GetPhysicsPhysicsComponents()[entity->_ComponentsIndex] };
	TransformComponent &transformComponent{ ComponentManager::GetPhysicsTransformComponents()[entity->_ComponentsIndex] };
	const PhysicsInitializationData *const RESTRICT physicsInitializationData{ static_cast<const PhysicsInitializationData *const RESTRICT>(data) };

	physicsComponent = physicsInitializationData->_PhysicsComponent;
	transformComponent = physicsInitializationData->_TransformComponent;

	//Destroy the initialization data.
	DestroyInitializationData<PhysicsInitializationData>(data);
}

/*
*	Initializes a point light entity.
*/
void EntityCreationSystem::InitializePointLightEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this point light entity.
	entity->_ComponentsIndex = ComponentManager::GetNewPointLightComponentsIndex(entity);

	//Copy the data over to the component.
	PointLightComponent &component{ ComponentManager::GetPointLightPointLightComponents()[entity->_ComponentsIndex] };
	const PointLightInitializationData *const RESTRICT pointLightInitializationData{ static_cast<const PointLightInitializationData *const RESTRICT>(data) };

	component._Enabled = pointLightInitializationData->_Enabled;
	component._Color = pointLightInitializationData->_Color;
	component._Position = pointLightInitializationData->_Position;
	component._Intensity = pointLightInitializationData->_Intensity;
	component._AttenuationDistance = pointLightInitializationData->_AttenuationDistance;

	//Destroy the initialization data.
	DestroyInitializationData<PointLightInitializationData>(data);
}

/*
*	Initializes a sound entity.
*/
void EntityCreationSystem::InitializeSoundEntity(Entity* const RESTRICT entity, EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	entity->_ComponentsIndex = ComponentManager::GetNewSoundComponentsIndex(entity);

	//Copy the data over to the component.
	SoundComponent &component{ ComponentManager::GetSoundSoundComponents()[entity->_ComponentsIndex] };
	const SoundInitializationData *const RESTRICT initializationData{ static_cast<const SoundInitializationData *const RESTRICT>(data) };

	component._Position = initializationData->_Position;

	//Create the sound instance.
	SoundSystem::Instance->CreateSoundInstance(initializationData->_Description, &component._Instance);

	//Set the position of the sound instance.
	SoundSystem::Instance->SetPosition(component._Instance, component._Position);

	//Play the instance.
	SoundSystem::Instance->Play(component._Instance);

	//Destroy the initialization data.
	DestroyInitializationData<SoundInitializationData>(data);
}

/*
*	Processes the termination queue.
*/
void EntityCreationSystem::ProcessTerminationQueue() NOEXCEPT
{
	//Lock the termination queue.
	ScopedWriteLock<Spinlock> scopedLock{ _TerminationQueueLock };

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
void EntityCreationSystem::TerminateDynamicPhysicalEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicPhysicalComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a particle system entity.
*/
void EntityCreationSystem::TerminateParticleSystemEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Terminate the particle system entity via the rendering system.
	RenderingSystem::Instance->TerminateParticleSystemEntity(entity);

	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a physics entity.
*/
void EntityCreationSystem::TerminatePhysicsEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnPhysicsComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a point light entity.
*/
void EntityCreationSystem::TerminatePointLightEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnPointLightComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Terminates a sound entity.
*/
void EntityCreationSystem::TerminateSoundEntity(Entity* const RESTRICT entity) NOEXCEPT
{
	//Destroy the sound instance.
	SoundSystem::Instance->DestroySoundInstance(ComponentManager::GetSoundSoundComponents()[entity->_ComponentsIndex]._Instance);

	//Return this entitiy's components index.
	ComponentManager::ReturnSoundComponentsIndex(entity->_ComponentsIndex);
}

/*
*	Processes the destruction queue.
*/
void EntityCreationSystem::ProcessDestructionQueue() NOEXCEPT
{
	//Lock the destruction queue.
	ScopedWriteLock<Spinlock> scopedLock{ _DestructionQueueLock };

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
void EntityCreationSystem::ProcessAutomaticTerminationQueue() NOEXCEPT
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
void EntityCreationSystem::ProcessAutomaticDestructionQueue() NOEXCEPT
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