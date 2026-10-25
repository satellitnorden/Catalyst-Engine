#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Path tracing.
#include <PathTracing/Object.h>

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
		MeshObject(const DynamicArray<Vertex> &vertices, const DynamicArray<uint32> &indices) NOEXCEPT;

		/*
		*	Returns the bounding box of this object.
		*/
		NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT override;

		/*
		*	Traces against this object.
		*	Should return if it hits, and if so, fill in the surface information.
		*/
		NO_DISCARD bool Trace(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT override;

	private:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

		//The bounding box.
		AxisAlignedBoundingBox3D _BoundingBox;

	};

}