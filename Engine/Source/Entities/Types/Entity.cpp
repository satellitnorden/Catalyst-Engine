//Header file.
#include <Entities/Types/Entity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/LightEntity.h>
#include <Entities/Types/SoundEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/PhysicsSystem.h>

//Static variable definitions.
constexpr Vector3<float> Entity::_DefaultPosition;
constexpr Vector3<float> Entity::_DefaultRotation;
constexpr Vector3<float> Entity::_DefaultScale;

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