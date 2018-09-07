//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

//Physics.
#include <Physics/PhysicalConstants.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system synchronously.
*/
void PhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	
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
		const float distanceToTerrain{ Vector3::LengthSquaredXZ(position - component->_TerrainUniformData.terrainPosition) };

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
	const float terrainSize{ terrainComponent._TerrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent._TerrainUniformData.terrainHeight };

	const float xIndex = (position._X - terrainComponent._TerrainUniformData.terrainPosition._X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position._Z - terrainComponent._TerrainUniformData.terrainPosition._Z + (terrainSize * 0.5f)) / terrainSize;

	return terrainProperties.At(xIndex, yIndex)._W * terrainHeight + terrainComponent._TerrainUniformData.terrainPosition._Y;
}

/*
*	Given a world position, returns the normal of the terrain at that point.
*/
Vector3 PhysicsSystem::GetTerrainNormalAtPosition(const Vector3 &position) const NOEXCEPT
{
	//For now, just use the first terrain properties there is.
	const TerrainComponent &terrainComponent{ ComponentManager::GetTerrainTerrainComponents()[0] };
	const CPUTexture2D &terrainProperties{ terrainComponent._TerrainProperties };
	const float terrainSize{ terrainComponent._TerrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent._TerrainUniformData.terrainHeight };

	const float xIndex = (position._X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position._Z + (terrainSize * 0.5f)) / terrainSize;

	const Vector4 &terrainProperty{ terrainProperties.At(xIndex, yIndex) };

	return Vector3(terrainProperty._X, terrainProperty._Y, terrainProperty._Z);
}