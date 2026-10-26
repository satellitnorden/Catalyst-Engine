//Header file.
#include <PathTracing/Objects/MeshObject.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	MeshObject::MeshObject(const DynamicArray<Vertex> &vertices, const DynamicArray<uint32> &indices) NOEXCEPT
	{
		//Set up the primitives.
		_Primitives.Reserve(indices.Size() / 3);

		for (uint64 index_index{ 0 }; index_index < indices.Size(); index_index += 3)
		{
			//Retrieve the vertices.
			const StaticArray<Vertex, 3> _vertices
			{
				vertices[indices[index_index + 0]],
				vertices[indices[index_index + 1]],
				vertices[indices[index_index + 2]]
			};

			//Add the primitive.
			_Primitives.Emplace(_vertices);
		}
		//Calculate the bounding box.
		for (const PathTracing::TrianglePrimitive &primitive : _Primitives)
		{
			_BoundingBox.Expand(primitive.BoundingBox());
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

		for (const PathTracing::TrianglePrimitive &primitive : _Primitives)
		{
			PathTracing::SurfaceInformation _surface_information;

			if (primitive.Trace(ray, &_surface_information) && _surface_information._HitDistance < closest_hit_distance)
			{
				*surface_information = _surface_information;
				
				closest_hit_distance = surface_information->_HitDistance;
				found_hit = true;
			}
		}

		return found_hit;
	}

}