//Header file.
#include <Physics/ModelPhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

/*
*	Casts a ray against dynamic models.
*/
void ModelPhysicsSystem::CastRayDynamicModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (Entity *const RESTRICT entity : *ComponentManager::GetDynamicModelEntities())
	{
		//Cache the relevant data.
		const AxisAlignedBoundingBox3 &world_space_axis_aligned_bounding_box{ *static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetWorldSpaceAxisAlignedBoundingBox() };
		const ResourcePointer<ModelResource> model_resource{ static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetModelResource() };
	
		//Cast the ray.
		float32 intersection_distance{ result->_HitDistance };

		if (CastRayModel(ray, world_space_axis_aligned_bounding_box, model_resource, static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetWorldTransform()->ToAbsoluteMatrix4x4(), &intersection_distance))
		{
			result->_HasHit = true;
			result->_HitDistance = intersection_distance;
			result->_Type = RaycastResult::Type::DYNAMIC_MODEL;
			result->_DynamicModelRaycastResult._Entity = entity;
		}
	}
}

/*
*	Casts a ray against static models.
*/
void ModelPhysicsSystem::CastRayStaticModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (Entity *const RESTRICT entity : *ComponentManager::GetStaticModelEntities())
	{
		//Cache the relevant data.
		const AxisAlignedBoundingBox3 &world_space_axis_aligned_bounding_box{ *static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetWorldSpaceAxisAlignedBoundingBox() };
		const ResourcePointer<ModelResource> model_resource{ static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetModelResource() };

		//Cast the ray.
		float32 intersection_distance{ result->_HitDistance };

		if (CastRayModel(ray, world_space_axis_aligned_bounding_box, model_resource, static_cast<const DynamicModelEntity *const RESTRICT>(entity)->GetWorldTransform()->ToAbsoluteMatrix4x4(), &intersection_distance))
		{
			result->_HasHit = true;
			result->_HitDistance = intersection_distance;
			result->_Type = RaycastResult::Type::STATIC_MODEL;
			result->_StaticModelRaycastResult._Entity = entity;
		}
	}
}

/*
*	Casts a ray against the given model.
*/
NO_DISCARD bool ModelPhysicsSystem::CastRayModel(	const Ray &ray,
													const AxisAlignedBoundingBox3 &world_space_axis_aligned_bounding_box,
													const ResourcePointer<ModelResource> model_resource,
													const Matrix4x4 &world_transform,
													float32 *const RESTRICT intersection_distance) NOEXCEPT
{
	//Cast a ray against the world space axis aligned bounding box to reject rays early that will never hit the model.
	{
		float32 world_space_axis_aligned_bounding_box_intersection_distance{ FLOAT_MAXIMUM };

		if (!CatalystGeometryMath::RayBoxIntersection(ray, world_space_axis_aligned_bounding_box, &world_space_axis_aligned_bounding_box_intersection_distance)
			|| *intersection_distance < world_space_axis_aligned_bounding_box_intersection_distance)
		{
			return false;
		}
	}

	//Now cast a ray against all triangles. (:
	bool hit_triangle{ false };

	{
		for (const Mesh &mesh : model_resource->_Meshes)
		{
			//Cache the mesh level of detail.
			const Mesh::MeshLevelOfDetail &mesh_level_of_detail{ mesh._MeshLevelOfDetails[0] };

			for (uint32 triangle_index{ 0 }; triangle_index < mesh_level_of_detail._IndexCount; triangle_index += 3)
			{
				//Construct the triangle and transform all vertices to world space.
				Triangle triangle;

				//Transform the vertices to world space.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					triangle._Vertices[i] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[triangle_index + i]]._Position;

					const Vector4<float32> world_space_vertex{ world_transform * Vector4<float32>(triangle._Vertices[i], 1.0f) };

					triangle._Vertices[i]._X = world_space_vertex._X;
					triangle._Vertices[i]._Y = world_space_vertex._Y;
					triangle._Vertices[i]._Z = world_space_vertex._Z;
				}

				//Cast the ray.
				float32 triangle_intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &triangle_intersection_distance)
					&& *intersection_distance > triangle_intersection_distance)
				{
					*intersection_distance = triangle_intersection_distance;

					hit_triangle = true;
				}
			}
		}
	}

	return hit_triangle;
}