#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entity types.
#define ENTITY_TYPES	ENTITY_TYPE(DynamicPhysical)	\
						ENTITY_TYPE(ParticleSystem)		\
						ENTITY_TYPE(Physics)			\
						ENTITY_TYPE(PointLight)			\
						ENTITY_TYPE(Sound)

//Enumeration covering all entity types.
enum class EntityType : uint8
{
#define ENTITY_TYPE(VALUE) VALUE,
	ENTITY_TYPES
#undef ENTITY_TYPE

	NumberOfEntityTypes
};