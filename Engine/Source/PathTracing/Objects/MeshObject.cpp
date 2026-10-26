//Header file.
#include <PathTracing/Objects/MeshObject.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	MeshObject::MeshObject(const PathTracing::Material *const RESTRICT material, const DynamicArray<Vertex> &vertices, const DynamicArray<uint32> &indices) NOEXCEPT
		:
		Object(material)
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
			_Primitives.Emplace(GetMaterial(), _vertices);
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
	*	Adds the primitives for this object to the given acceleration structure.
	*/
	void MeshObject::AddPrimitives(PathTracing::AccelerationStructure *const RESTRICT acceleration_structure) const NOEXCEPT
	{
		for (const PathTracing::TrianglePrimitive &primitive : _Primitives)
		{
			acceleration_structure->AddPrimitive(&primitive);
		}
	}

}