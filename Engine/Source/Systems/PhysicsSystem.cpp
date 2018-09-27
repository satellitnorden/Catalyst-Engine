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
	//Do a simple ray-box intersection test to determine which dynamic physical entity was hit.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	const FrustumCullingComponent *RESTRICT component{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents() };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++component)
	{
		const AxisAlignedBoundingBox &box{ component->_WorldSpaceAxisAlignedBoundingBox };

		if (CatalystVectorMath::LineBoxIntersection(box, ray))
		{
			result->_HasHit = true;
			result->_HitEntity = ComponentManager::GetDynamicPhysicalEntities()->At(i);

			return;
		}
	}

	//If there was not hit, update the result.
	result->_HasHit = false;
	result->_HitEntity = nullptr;
}