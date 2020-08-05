#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class AxisAlignedBoundingBox3;
class Entity;

//Type aliases.
using EntityPlacementFunction = void(*)(const AxisAlignedBoundingBox3 &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities);

//Enumeration covering all entity placement types.
enum class EntityPlacementType : uint8
{
	TwoDimensional,
	ThreeDimensional
};

//Entity types.
#define ENTITY_TYPES	ENTITY_TYPE(AnimatedModel)			\
						ENTITY_TYPE(DistanceTrigger)		\
						ENTITY_TYPE(DynamicModel)			\
						ENTITY_TYPE(InstancedStaticModel)	\
						ENTITY_TYPE(Light)					\
						ENTITY_TYPE(ParticleSystem)			\
						ENTITY_TYPE(Sound)					\
						ENTITY_TYPE(StaticModel)			\
						ENTITY_TYPE(Vegetation)

//Enumeration covering all entity types.
enum class EntityType : uint8
{
#define ENTITY_TYPE(VALUE) VALUE,
	ENTITY_TYPES
#undef ENTITY_TYPE

	NumberOfEntityTypes
};