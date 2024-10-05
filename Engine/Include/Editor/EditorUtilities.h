#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Assets/ModelAsset.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

namespace EditorUtilities
{

	/*
	*	Casts a ray against the given model.
	*/
	FORCE_INLINE NO_DISCARD bool SelectionRay(const Ray &ray, const Matrix4x4 &model_transform, const ModelAsset *const RESTRICT model, float32 *const RESTRICT hit_distance) NOEXCEPT
	{
		//Set up the axis aligned bounding box.
		AxisAlignedBoundingBox3D axis_aligned_bounding_box{ model->_ModelSpaceAxisAlignedBoundingBox };

		for (uint8 i{ 0 }; i < 2; ++i)
		{
			const Vector4<float32> transformed_position{ model_transform * Vector4<float32>(axis_aligned_bounding_box[i]) };
		}

		//Check if the ray hit the axis aligned bounding box.
		float32 axis_aligned_bounding_box_hit_distance{ *hit_distance };

		if (CatalystGeometryMath::RayBoxIntersection(ray, axis_aligned_bounding_box, &axis_aligned_bounding_box_hit_distance) && *hit_distance > axis_aligned_bounding_box_hit_distance)
		{
			//Now actually ray cast against all the triangles. (:
			bool was_hit{ false };

			for (uint64 mesh_index{ 0 }; mesh_index < model->_Meshes.Size(); ++mesh_index)
			{
				//Cache the mesh.
				const Mesh& mesh{ model->_Meshes[mesh_index] };

				//Cache the mesh level of detail.
				const Mesh::MeshLevelOfDetail& mesh_level_of_detail{ mesh._MeshLevelOfDetails[0] };

				//Ray-cast against all triangles.
				for (uint64 index_index{ 0 }; index_index < mesh_level_of_detail._Indices.Size(); index_index += 3)
				{
					StaticArray<Vertex, 3> vertices;

					vertices[0] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index_index + 0]];
					vertices[1] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index_index + 1]];
					vertices[2] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index_index + 2]];

					vertices[0].Transform(model_transform, 0.0f);
					vertices[1].Transform(model_transform, 0.0f);
					vertices[2].Transform(model_transform, 0.0f);

					Triangle triangle;

					triangle._Vertices[0] = vertices[0]._Position;
					triangle._Vertices[1] = vertices[1]._Position;
					triangle._Vertices[2] = vertices[2]._Position;

					float32 triangle_hit_distance{ FLOAT32_MAXIMUM };

					if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &triangle_hit_distance) && *hit_distance > triangle_hit_distance)
					{
						*hit_distance = triangle_hit_distance;
						was_hit = true;
					}
				}
			}

			return was_hit;
		}

		//Didn't hit. (:
		return false;
	}
}