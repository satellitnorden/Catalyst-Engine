//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>
#include <Math/Vector3.h>

//Physics.
#include <Physics/PhysicalConstants.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system synchronously during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	
}

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{
	//Pre-calculate the inverse direction of the ray to avoid costly divisions.
	const Vector3 inverseDirection{ Vector3(1.0f) / ray._Direction };

	//Do a simple ray-box intersection test to determine which dynamic physical entity was hit.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	const FrustumCullingComponent *RESTRICT component{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents() };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++component)
	{
		const AxisAlignedBoundingBox &box{ component->_WorldSpaceAxisAlignedBoundingBox };

		if (CatalystVectorMath::LineBoxIntersection(box, ray))
		{
			result->_HasHit = true;

			return;
		}
	}

	//If there was not hit, update the result.
	result->_HasHit = false;
}

/*
*	Given a world position, returns the height of the terrain at that point.
*/
float PhysicsSystem::GetTerrainHeightAtPosition(const Vector3 &position) const NOEXCEPT
{
	//Calculate the nearest terrain.
	const TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };
	const uint64 numberOfComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	const TerrainComponent *RESTRICT nearestComponent{ nullptr };
	float nearestDistance{ FLOAT_MAXIMUM };

	for (uint64 i = 0; i < numberOfComponents; ++i, ++component)
	{
		const float distanceToTerrain{ Vector3::LengthSquaredXZ(position - component->_TerrainUniformData._TerrainPosition) };

		if (distanceToTerrain < nearestDistance)
		{
			nearestComponent = component;
			nearestDistance = distanceToTerrain;
		}
	}

	//If a nearest component could not be found, just return zero.
	if (!nearestComponent)
	{
		return 0.0f;
	}

	const TerrainComponent &terrainComponent{ *nearestComponent };
	const CPUTexture2D &terrainProperties{ terrainComponent._TerrainProperties };
	const float terrainSize{ terrainComponent._TerrainUniformData._TerrainSize };
	const float terrainHeight{ terrainComponent._TerrainUniformData._TerrainHeight };

	const float xIndex = (position._X - terrainComponent._TerrainUniformData._TerrainPosition._X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position._Z - terrainComponent._TerrainUniformData._TerrainPosition._Z + (terrainSize * 0.5f)) / terrainSize;

	return terrainProperties.At(xIndex, yIndex)._W * terrainHeight + terrainComponent._TerrainUniformData._TerrainPosition._Y;
}

/*
*	Given a world position, returns the normal of the terrain at that point.
*/
Vector3 PhysicsSystem::GetTerrainNormalAtPosition(const Vector3 &position) const NOEXCEPT
{
	//For now, just use the first terrain properties there is.
	const TerrainComponent &terrainComponent{ ComponentManager::GetTerrainTerrainComponents()[0] };
	const CPUTexture2D &terrainProperties{ terrainComponent._TerrainProperties };
	const float terrainSize{ terrainComponent._TerrainUniformData._TerrainSize };
	const float terrainHeight{ terrainComponent._TerrainUniformData._TerrainHeight };

	const float xIndex = (position._X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position._Z + (terrainSize * 0.5f)) / terrainSize;

	const Vector4 &terrainProperty{ terrainProperties.At(xIndex, yIndex) };

	return Vector3(terrainProperty._X, terrainProperty._Y, terrainProperty._Z);
}