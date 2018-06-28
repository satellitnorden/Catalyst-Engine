//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector3.h>

//Physics.
#include <Physics/PhysicalConstants.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system synchronously.
*/
void PhysicsSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	
}

/*
*	Given a world position, returns the height of the terrain at that point.
*/
float PhysicsSystem::GetTerrainHeightAtPosition(const Vector3 &position) const NOEXCEPT
{
	//Calculate the nearest terrain.
	const TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainComponents() };
	const uint64 numberOfComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	const TerrainComponent *RESTRICT nearestComponent{ nullptr };
	float nearestDistance{ FLOAT_MAXIMUM };

	for (uint64 i = 0; i < numberOfComponents; ++i, ++component)
	{
		const float distanceToTerrain{ Vector3::LengthSquaredXZ(position - component->terrainUniformData.terrainPosition) };

		if (distanceToTerrain < nearestDistance)
		{
			nearestComponent = component;
			nearestDistance = distanceToTerrain;
		}
	}

	const TerrainComponent &terrainComponent{ *component };
	const CPUTexture2D &terrainProperties{ terrainComponent.terrainProperties };
	const float terrainSize{ terrainComponent.terrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent.terrainUniformData.terrainHeight };

	const float xIndex = (position.X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position.Z + (terrainSize * 0.5f)) / terrainSize;

	return terrainProperties.At(xIndex, yIndex).W * terrainHeight + terrainComponent.terrainUniformData.terrainPosition.Y;
}

/*
*	Given a world position, returns the normal of the terrain at that point.
*/
Vector3 PhysicsSystem::GetTerrainNormalAtPosition(const Vector3 &position) const NOEXCEPT
{
	//For now, just use the first terrain properties there is.
	const TerrainComponent &terrainComponent{ ComponentManager::GetTerrainComponents()[0] };
	const CPUTexture2D &terrainProperties{ terrainComponent.terrainProperties };
	const float terrainSize{ terrainComponent.terrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent.terrainUniformData.terrainHeight };

	const float xIndex = (position.X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position.Z + (terrainSize * 0.5f)) / terrainSize;

	const Vector4 &terrainProperty{ terrainProperties.At(xIndex, yIndex) };

	return Vector3(terrainProperty.X, terrainProperty.Y, terrainProperty.Z);
}