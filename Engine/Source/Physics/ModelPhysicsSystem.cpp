//Header file.
#include <Physics/ModelPhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

/*
*	Registers dynamic model collision configuration.
*/
void ModelPhysicsSystem::RegisterDynamicModelCollisionConfiguration(DynamicModelEntity *const RESTRICT entity, const ModelCollisionConfiguration &configuration) NOEXCEPT
{
	if (ModelCollisionConfiguration *const RESTRICT existing_configuration{ _DynamicModelCollisionConfigurations.Find(entity) })
	{
		*existing_configuration = configuration;
	}

	else
	{
		_DynamicModelCollisionConfigurations.Add(entity, configuration);
	}
}

/*
*	Registers static model collision configuration.
*/
void ModelPhysicsSystem::RegisterStaticModelCollisionConfiguration(StaticModelEntity *const RESTRICT entity, const ModelCollisionConfiguration &configuration) NOEXCEPT
{
	if (ModelCollisionConfiguration *const RESTRICT existing_configuration{ _StaticModelCollisionConfigurations.Find(entity) })
	{
		*existing_configuration = configuration;
	}

	else
	{
		_StaticModelCollisionConfigurations.Add(entity, configuration);
	}
}

/*
*	Casts a ray against dynamic models.
*/
void ModelPhysicsSystem::CastRayDynamicModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (uint64 i{ 0 }, size{ _DynamicModelCollisionConfigurations.Size() }; i < size; ++i)
	{
		switch (_DynamicModelCollisionConfigurations.ValueAt(i)._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				float intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, *_DynamicModelCollisionConfigurations.KeyAt(i)->GetWorldSpaceAxisAlignedBoundingBox(), &intersection_distance)
					&& result->_HitDistance > intersection_distance)
				{
					result->_HasHit = true;
					result->_HitDistance = intersection_distance;
					result->_Type = RaycastResult::Type::DYNAMIC_MODEL;
					result->_DynamicModelRaycastResult._Entity = _DynamicModelCollisionConfigurations.KeyAt(i);
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
	for (uint64 i{ 0 }, size{ _StaticModelCollisionConfigurations.Size() }; i < size; ++i)
	{
		switch (_StaticModelCollisionConfigurations.ValueAt(i)._Type)
		{
		case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
		{
			float intersection_distance{ FLOAT_MAXIMUM };

			if (CatalystGeometryMath::RayBoxIntersection(ray, *_StaticModelCollisionConfigurations.KeyAt(i)->GetWorldSpaceAxisAlignedBoundingBox(), &intersection_distance)
				&& result->_HitDistance > intersection_distance)
			{
				result->_HasHit = true;
				result->_HitDistance = intersection_distance;
				result->_Type = RaycastResult::Type::STATIC_MODEL;
				result->_DynamicModelRaycastResult._Entity = _StaticModelCollisionConfigurations.KeyAt(i);
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