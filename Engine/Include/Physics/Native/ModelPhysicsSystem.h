#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/Native/ModelPhysicsSimulationData.h>
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/RaycastConfiguration.h>
#include <Physics/Native/RaycastResult.h>

class ModelPhysicsSystem final
{

public:

	/*
	*	Casts a ray against dynamic models.
	*/
	void CastRayDynamicModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against static models.
	*/
	void CastRayStaticModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

private:

	/*
	*	Casts a ray against the given model.
	*/
	NO_DISCARD bool CastRayModel(	const Ray &ray,
									const AxisAlignedBoundingBox3D &world_space_axis_aligned_bounding_box,
									const ResourcePointer<ModelResource> model_resource,
									const Matrix4x4 &world_transform,
									float32 *const RESTRICT intersection_distance) NOEXCEPT;

};