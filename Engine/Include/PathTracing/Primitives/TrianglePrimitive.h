#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/Triangle.h>

//Path tracing.
#include <PathTracing/Primitive.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

namespace PathTracing
{

	/*
	*	Class representing a triangle primitive.
	*/
	class TrianglePrimitive final : public PathTracing::Primitive
	{

	public:

		/*
		*	Default constructor.
		*/
		TrianglePrimitive(const StaticArray<Vertex, 3> &vertices) NOEXCEPT;

		/*
		*	Returns the center of this primitive.
		*/
		NO_DISCARD Vector3<float32> Center() const NOEXCEPT override;

		/*
		*	Returns the bounding box of this primitive.
		*/
		NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT override;

		/*
		*	Traces a surface ray against this primitive.
		*	Should return if it hits, and if so, fill in the surface information.
		*/
		NO_DISCARD bool TraceSurface(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT override;

		/*
		*	Traces an occlusion ray against this primitive.
		*	Should return if it hits.
		*/
		NO_DISCARD bool TraceOcclusion(const Ray &ray) const NOEXCEPT override;

	private:

		//The triangle.
		Triangle _Triangle;

		//The normals.
		StaticArray<Vector3<float32>, 3> _Normals;

	};

}