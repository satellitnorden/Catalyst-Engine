//Header file.
#include <Entities/Types/Entity.h>

//Entities.
#include <Entities/Types/AnimatedModelEntity.h>
#include <Entities/Types/DistanceTriggerEntity.h>
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/InstancedImpostorEntity.h>
#include <Entities/Types/InstancedStaticModelEntity.h>
#include <Entities/Types/LightEntity.h>
#include <Entities/Types/ParticleSystemEntity.h>
#include <Entities/Types/SoundEntity.h>
#include <Entities/Types/StaticModelEntity.h>
#include <Entities/Types/TerrainEntity.h>
#include <Entities/Types/UserInterfaceEntity.h>

/*
*	Returns the preprocessing parameters.
*/
void Entity::GetPreprocessingParameters(EntityPreprocessingParameters *const RESTRICT parameters) NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:														\
		{																								\
			static_cast<VALUE##Entity *const RESTRICT>(this)->GetPreprocessingParameters(parameters);	\
																										\
			break;																						\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}
}

/*
*	Preprocesses this entity.
*/
void Entity::Preprocess(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:								\
		{																		\
			static_cast<VALUE##Entity *const RESTRICT>(this)->Preprocess(data);	\
																				\
			break;																\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}
}

/*
*	Initializes this entity.
*/
void Entity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:								\
		{																		\
			static_cast<VALUE##Entity *const RESTRICT>(this)->Initialize(data);	\
																				\
			break;																\
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
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:									\
		{																			\
			static_cast< VALUE##Entity *const RESTRICT>(this)->Terminate();	\
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
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:															\
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

/*
*	Returns the initialization data required to duplicate this entity.
*/
RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT Entity::GetDuplicationInitializationData() const NOEXCEPT
{
	switch (_Type)
	{
#define ENTITY_TYPE(VALUE) case EntityType::VALUE:																\
		{																										\
			return static_cast<const VALUE ## Entity *const RESTRICT>(this)->GetDuplicationInitializationData();\
																												\
			break;																								\
		}
		ENTITY_TYPES
#undef ENTITY_TYPE
	}

	ASSERT(false, "What happened here?");

	return nullptr;
}