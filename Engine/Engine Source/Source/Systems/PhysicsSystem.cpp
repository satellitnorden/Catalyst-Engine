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
*	Default constructor.
*/
PhysicsSystem::PhysicsSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
PhysicsSystem::~PhysicsSystem() NOEXCEPT
{

}

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
	//For now, just use the first terrain properties there is.
	const TerrainComponent &terrainComponent{ ComponentManager::GetTerrainComponents()[0] };
	const CPUTexture4 &terrainProperties{ terrainComponent.terrainProperties };
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
	const CPUTexture4 &terrainProperties{ terrainComponent.terrainProperties };
	const float terrainSize{ terrainComponent.terrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent.terrainUniformData.terrainHeight };

	const float xIndex = (position.X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position.Z + (terrainSize * 0.5f)) / terrainSize;

	const Vector4 &terrainProperty{ terrainProperties.At(xIndex, yIndex) };

	return Vector3(terrainProperty.X, terrainProperty.Y, terrainProperty.Z);
}