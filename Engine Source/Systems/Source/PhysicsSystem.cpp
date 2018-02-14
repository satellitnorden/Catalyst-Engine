//Header file.
#include <PhysicsSystem.h>

//Components.
#include <ComponentManager.h>

//Entities.
#include <Entity.h>

//Math.
#include <GameMath.h>
#include <Vector3.h>

//Physics.
#include <PhysicalConstants.h>

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
	//For now, just use the first terrain height map there is.
	const TerrainComponent &terrainComponent{ ComponentManager::GetTerrainEntityTerrainComponents()[0] };
	const CPUTexture4 &heightMap{ terrainComponent.heightMap };
	const float terrainSize{ terrainComponent.terrainUniformData.terrainSize };
	const float terrainHeight{ terrainComponent.terrainUniformData.terrainHeight };

	const float xIndex = (position.X + (terrainSize * 0.5f)) / terrainSize;
	const float yIndex = (position.Z + (terrainSize * 0.5f)) / terrainSize;

	return heightMap.At(xIndex, yIndex).X * terrainHeight;
}