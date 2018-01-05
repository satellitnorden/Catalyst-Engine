//Header file.
#include <SpotLightEntity.h>

//Static variable definitions.
DynamicArray<SpotLightEntity *CATALYST_RESTRICT> SpotLightEntity::spotLightEntities;

/*
*	Default constructor.
*/
SpotLightEntity::SpotLightEntity() CATALYST_NOEXCEPT
{
	//Add this spot light entity to the universal container.
	spotLightEntities.Emplace(this);
}

/*
*	Default destructor.
*/
SpotLightEntity::~SpotLightEntity() CATALYST_NOEXCEPT
{
	//Remove this spot light entity from the universal container.
	spotLightEntities.Erase(this);
}

/*
*	Returns the direction of this spot light.
*/
Vector3 SpotLightEntity::GetDirection() const CATALYST_NOEXCEPT
{
	Vector3 direction{ 0.0f, -1.0f, 0.0f };

	direction.Rotate(GetWorldRotation());
	direction.Y *= -1.0f;

	return direction;
}