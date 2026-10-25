//Header file.
#include <PathTracing/Objects/MeshObject.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/Triangle.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	MeshObject::MeshObject(const DynamicArray<Vertex> &vertices, const DynamicArray<uint32> &indices) NOEXCEPT
		:
		_Vertices(vertices),
		_Indices(indices)
	{
		//Calculate the bounding box.
		for (const Vertex &vertex : _Vertices)
		{
			_BoundingBox.Expand(vertex._Position);
		}
	}

	/*
	*	Returns the bounding box of this object.
	*/
	NO_DISCARD AxisAlignedBoundingBox3D MeshObject::BoundingBox() const NOEXCEPT
	{
		return _BoundingBox;
	}

	/*
	*	Traces against this object.
	*	Should return if it hits, and if so, fill in the surface information.
	*/
	NO_DISCARD bool MeshObject::Trace(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT
	{
		//Trace against each triangle.
		float32 closest_hit_distance{ FLOAT32_MAXIMUM };
		bool found_hit{ false };

		for (uint64 index_index{ 0 }; index_index < _Indices.Size(); index_index += 3)
		{
			//Retrieve the vertices.
			const StaticArray<Vertex, 3> vertices
			{
				_Vertices[_Indices[index_index + 0]],
				_Vertices[_Indices[index_index + 1]],
				_Vertices[_Indices[index_index + 2]]
			};

			//Construct the triangle.
			Triangle triangle;

			triangle._Vertices[0] = vertices[0]._Position;
			triangle._Vertices[1] = vertices[1]._Position;
			triangle._Vertices[2] = vertices[2]._Position;

			//Check if there is an intersection.
			float32 hit_distance{ FLOAT32_MAXIMUM };

			if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &hit_distance) && hit_distance < closest_hit_distance)
			{
				//Calculate the hit position.
				const Vector3<float32> hit_position{ ray._Origin + ray._Direction * hit_distance };

				//Calculate the barycentric coordinates.
				const Vector3<float32> barcentric_coordinates{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_position) };

				//Fill in the surface information.
				surface_information->_HitDistance = hit_distance;
				surface_information->_Position = ray._Origin + ray._Direction * hit_distance;
				surface_information->_Normal =	vertices[0]._Normal * barcentric_coordinates[0]
												+ vertices[1]._Normal * barcentric_coordinates[1]
												+ vertices[2]._Normal * barcentric_coordinates[2];
				surface_information->_Normal.Normalize();

				closest_hit_distance = hit_distance;
				found_hit = true;
			}
		}

		return found_hit;
	}

}