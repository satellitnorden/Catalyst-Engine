//Header file.
#include <Physics/ModelPhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

/*
*	Registers dynamic model collision data.
*/
void ModelPhysicsSystem::RegisterDynamicModelCollisionData(const uint64 entity_identifier, const ModelCollisionData &data) NOEXCEPT
{
	_DynamicModelCollisionData[entity_identifier] = data;
}

/*
*	Unregisters dynamic model collision data.
*/
void ModelPhysicsSystem::UnregisterDynamicModelCollisionData(const uint64 entity_identifier) NOEXCEPT
{

}

/*
*	Registers static model collision data.
*/
void ModelPhysicsSystem::RegisterStaticModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT
{
	_StaticModelCollisionData[entity_identifier] = data;
}

/*
*	Unregisters static model collision data.
*/
void ModelPhysicsSystem::UnregisterStaticModelCollisionData(const uint64 entity_identifer) NOEXCEPT
{

}

/*
*	Casts a ray against dynamic models.
*/
void ModelPhysicsSystem::CastRayDynamicModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (const Pair<uint64, ModelCollisionData>& data : _DynamicModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				float intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox, &intersection_distance)
					&& result->_HitDistance > intersection_distance)
				{
					result->_HasHit = true;
					result->_HitDistance = intersection_distance;
					result->_Type = RaycastResult::Type::DYNAMIC_MODEL;
					result->_DynamicModelRaycastResult._Entity = ComponentManager::GetDynamicModelEntities()->At(data._First);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}

/*
*	Casts a ray against static models.
*/
void ModelPhysicsSystem::CastRayStaticModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (const Pair<uint64, ModelCollisionData>& data : _StaticModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				float intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox, &intersection_distance)
					&& result->_HitDistance > intersection_distance)
				{
					result->_HasHit = true;
					result->_HitDistance = intersection_distance;
					result->_Type = RaycastResult::Type::STATIC_MODEL;
					result->_DynamicModelRaycastResult._Entity = ComponentManager::GetStaticModelEntities()->At(data._First);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}