//Header file.
#include <PathTracing/Objects/MeshObject.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	MeshObject::MeshObject
	(
		const PathTracing::Transform &transform,
		const PathTracing::Material *const RESTRICT material,
		const DynamicArray<Vertex> &vertices,
		const DynamicArray<uint32> &indices
	) NOEXCEPT
		:
		Object(transform, material)
	{
		//Set up the primitives.
		_Primitives.Reserve(indices.Size() / 3);

		for (uint64 index_index{ 0 }; index_index < indices.Size(); index_index += 3)
		{
			//Retrieve the vertices.
			StaticArray<Vertex, 3> _vertices
			{
				vertices[indices[index_index + 0]],
				vertices[indices[index_index + 1]],
				vertices[indices[index_index + 2]]
			};

			//Transform the positions.
			for (uint8 i{ 0 }; i < 3; ++i)
			{
				_vertices[i]._Position = GetTransform().TransformPosition(_vertices[i]._Position);
			}

			//Transform the normals.
			for (uint8 i{ 0 }; i < 3; ++i)
			{
				_vertices[i]._Normal = GetTransform().TransformDirection(_vertices[i]._Normal);
			}

			//Transform the tangents.
			for (uint8 i{ 0 }; i < 3; ++i)
			{
				_vertices[i]._Tangent = GetTransform().TransformDirection(_vertices[i]._Tangent);
			}

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