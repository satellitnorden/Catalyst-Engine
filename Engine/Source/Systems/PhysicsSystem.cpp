//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : _CharacterMovements)
	{
		//Apply the movement.
		movement->_Position += movement->_Velocity * context->_DeltaTime;
		movement->_Velocity = VectorConstants::ZERO;
	}
}

/*
*	Registers a character movement.
*/
void PhysicsSystem::RegisterCharacterMovement(CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	_CharacterMovements.EmplaceSlow(movement);
}

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const Ray &ray, const PhysicsChannel channels, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Set the default properties of the result.
	result->_HasHit = false;
	result->_HitDistance = FLOAT_MAXIMUM;
	result->_HitEntity = nullptr;

	//Raycast against the terrain.
	if (TEST_BIT(channels, PhysicsChannel::Terrain))
	{
		CastRayTerrain(ray, result);
	}

	//Raycast against models.
	if (TEST_BIT(channels, PhysicsChannel::Model))
	{

	}
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT
{
	float intersectionDistance;

	if (CatalystGeometryMath::RayPlaneIntersection(ray,
													Plane(VectorConstants::ZERO, VectorConstants::UP),
													&intersectionDistance))
	{
		if (intersectionDistance < result->_HitDistance)
		{
			result->_HasHit = true;
			result->_HitDistance = intersectionDistance;
			result->_HitEntity = nullptr;
		}
	}
}