#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using EntityIdentifier = uint64;

//Entity constants.
namespace EntityConstants
{
	constexpr uint64 MAXIMUM_NUMBER_OF_COMPONENTS_PER_ENTITY{ 4 };
}

//Forward declarations.
class AxisAlignedBoundingBox3D;
class Entity;

//Entity types.
#define ENTITY_TYPES	ENTITY_TYPE(AnimatedModel)			\
						ENTITY_TYPE(Light)					\
						ENTITY_TYPE(ParticleSystem)			\
						ENTITY_TYPE(UserInterface)

//Enumeration covering all entity types.
enum class EntityType : uint8
{
#define ENTITY_TYPE(VALUE) VALUE,
	ENTITY_TYPES
#undef ENTITY_TYPE

	NumberOfEntityTypes
};