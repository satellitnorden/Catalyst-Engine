//Header file.
#include <Entities/Types/Entity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/AnimatedModelEntity.h>
#include <Entities/Types/DistanceTriggerEntity.h>
#include <Entities/Types/LightEntity.h>
#include <Entities/Types/ModelEntity.h>
#include <Entities/Types/ParticleSystemEntity.h>
#include <Entities/Types/SoundEntity.h>
#include <Entities/Types/VegetationEntity.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/PhysicsSystem.h>

/*
*	Initializes this entity.
*/
void Entity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::## VALUE ## :								\
		{																				\
			static_cast< ## VALUE ## Entity *const RESTRICT>(this)->Initialize(data);	\
																						\
			break;																		\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}
}

/*
*	Terminates this entity.
*/
void Entity::Terminate() NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::## VALUE ## :							\
		{																			\
			static_cast< ## VALUE ## Entity *const RESTRICT>(this)->Terminate();	\
																					\
			break;																	\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}
}

/*
*	Returns whether or not this entity should automatically terminate.
*/
bool Entity::ShouldAutomaticallyTerminate() const NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::## VALUE ## :													\
		{																									\
			return static_cast<const VALUE ## Entity *const RESTRICT>(this)->ShouldAutomaticallyTerminate();\
																											\
			break;																							\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}

	ASSERT(false, "What happened here?");

	return false;
}