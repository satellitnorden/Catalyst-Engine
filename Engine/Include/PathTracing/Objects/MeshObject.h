#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Path tracing.
#include <PathTracing/Object.h>
#include <PathTracing/Primitives/TrianglePrimitive.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

namespace PathTracing
{

	/*
	*	Mesh object class.
	*	Represents a triangle mesh.
	*/
	class MeshObject final : public PathTracing::Object
	{

	public:

		/*
		*	Default constructor.
		*/
		MeshObject(const PathTracing::Material *const RESTRICT material, const DynamicArray<Vertex> &vertices, const DynamicArray<uint32> &indices) NOEXCEPT;

		/*
		*	Returns the bounding box of this object.
		*/
		NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT override;

		/*
		*	Adds the primitives for this object to the given acceleration structure.
		*/
		void AddPrimitives(PathTracing::AccelerationStructure *const RESTRICT acceleration_structure) const NOEXCEPT override;

	private:

		//The primitives.
		DynamicArray<PathTracing::TrianglePrimitive> _Primitives;

		//The bounding box.
		AxisAlignedBoundingBox3D _BoundingBox;

	};

}