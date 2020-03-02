//Header file.
#include <Physics/ModelPhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

/*
*	Registers model collision data.
*/
void ModelPhysicsSystem::RegisterModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT
{
	_ModelCollisionData[entity_identifier] = data;
}

/*
*	Unregisters model collision data.
*/
void ModelPhysicsSystem::UnregisterModelCollisionData(const uint64 entity_identifer) NOEXCEPT
{

}

/*
*	Casts a ray against models.
*/
void ModelPhysicsSystem::CastRayModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT
{
	for (const Pair<uint64, ModelCollisionData>& data : _ModelCollisionData)
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
				result->_Type = RaycastResult::Type::MODEL;
			}

			break;
		}
		}
	}
}