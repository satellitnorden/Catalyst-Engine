//Header file.
#include <Physics/ModelPhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

/*
*	Updates the model physics system during the physics update phase.
*/
void ModelPhysicsSystem::PhysicsUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Simulate physics for all models.
	for (Pair<uint64, ModelPhysicsSimulationData> &model_physics_simulation_data : _ModelPhysicsSimulationData)
	{
		SimulatePhysics(context, &model_physics_simulation_data);
	}
}

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
*	Registers model physics simulation data.
*/
void ModelPhysicsSystem::RegisterModelPhysicsSimulationData(const uint64 entity_identifier, const ModelPhysicsSimulationData &data) NOEXCEPT
{
	_ModelPhysicsSimulationData[entity_identifier] = data;
}

/*
*	Unregisters model physics simulation data.
*/
void ModelPhysicsSystem::UnregisterModelPhysicsSimulationData(const uint64 entity_identifier) NOEXCEPT
{

}

/*
*	Casts a ray against models.
*/
void ModelPhysicsSystem::CastRayModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
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

/*
*	Simulates physics for one model.
*/
void ModelPhysicsSystem::SimulatePhysics(const UpdateContext *const RESTRICT context, Pair<uint64, ModelPhysicsSimulationData>* const RESTRICT data) NOEXCEPT
{

}