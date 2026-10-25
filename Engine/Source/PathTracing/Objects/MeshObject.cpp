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
	*	Should return if it hits, and if so, fill in the passed in result.
	*/
	NO_DISCARD bool MeshObject::Trace(const Ray &ray) const NOEXCEPT
	{
		//Trace against each triangle.
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
			if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle))
			{
				return true;
			}
		}

		//None hit.
		return false;
	}

}