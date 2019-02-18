//Header file.
#include <Entities/Types/PhysicsEntity.h>

/*
*	Default constructor.
*/
PhysicsEntity::PhysicsEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Physics;
}