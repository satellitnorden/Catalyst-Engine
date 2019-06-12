//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystPhysicsMath.h>
#include <Math/General/Vector.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

//Define the physics system.
#if defined(CATALYST_PHYSICS_PHYSX)
	#include <Physics/Abstraction/PhysX/PhysXPhysicsSystem.h>
	#define PHYSICS_SYSTEM PhysXPhysicsSystem
#endif