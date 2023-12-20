#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class AxisAlignedBoundingBox3D;
class Entity;

//Entity types.
#define ENTITY_TYPES	ENTITY_TYPE(AnimatedModel)			\
						ENTITY_TYPE(DistanceTrigger)		\
						ENTITY_TYPE(DynamicModel)			\
						ENTITY_TYPE(Grass)					\
						ENTITY_TYPE(InstancedImpostor)		\
						ENTITY_TYPE(InstancedStaticModel)	\
						ENTITY_TYPE(Light)					\
						ENTITY_TYPE(ParticleSystem)			\
						ENTITY_TYPE(Sound)					\
						ENTITY_TYPE(StaticModel)			\
						ENTITY_TYPE(Terrain)				\
						ENTITY_TYPE(UserInterface)

//Enumeration covering all entity types.
enum class EntityType : uint8
{
#define ENTITY_TYPE(VALUE) VALUE,
	ENTITY_TYPES
#undef ENTITY_TYPE

	NumberOfEntityTypes
};