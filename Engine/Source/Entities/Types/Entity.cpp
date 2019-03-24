//Header file.
#include <Entities/Types/Entity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/CollisionEntity.h>
#include <Entities/Types/DynamicPhysicalEntity.h>
#include <Entities/Types/ParticleSystemEntity.h>
#include <Entities/Types/PhysicsEntity.h>
#include <Entities/Types/PointLightEntity.h>
#include <Entities/Types/SoundEntity.h>

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
*	Returns the local transform of this entity, non-const.
*/
Matrix4 *const RESTRICT Entity::GetLocalTransform() NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::## VALUE ## :														\
		{																										\
			return &ComponentManager::Get ## VALUE ## TransformComponents()[_ComponentsIndex]._LocalTransform;	\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}

	ASSERT(false, "What happened here?");

	return nullptr;
}

/*
*	Returns the world transform of this entity, non-const.
*/
Matrix4 *const RESTRICT Entity::GetWorldTransform() NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::## VALUE ## :														\
		{																										\
			return &ComponentManager::Get ## VALUE ## TransformComponents()[_ComponentsIndex]._WorldTransform;	\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}

	ASSERT(false, "What happened here?");

	return nullptr;
}

/*
*	Transforms this entity.
*/
void Entity::Transform(const Matrix4 &transformation) NOEXCEPT
{
	if (_Initialized)
	{
		Matrix4 *const RESTRICT localTransform{ GetLocalTransform() };
		Matrix4 *const RESTRICT worldTransform{ GetWorldTransform() };

		*worldTransform = transformation * *localTransform;
	}
}