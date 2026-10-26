#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Path tracing.
#include <PathTracing/SurfaceInformation.h>

namespace PathTracing
{

	/*
	*	Class representing one primitivve shape in a path tracing context.
	*	Child classes should expose information about this primitive, and add functions for tracing against it.
	*/
	class Primitive
	{

	public:

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Primitive() NOEXCEPT
		{

		}

		/*
		*	Returns the center of this primitive.
		*/
		virtual NO_DISCARD Vector3<float32> Center() const NOEXCEPT = 0;

		/*
		*	Returns the bounding box of this primitive.
		*/
		virtual NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT = 0;

		/*
		*	Traces against this primitive.
		*	Should return if it hits, and if so, fill in the surface information.
		*/
		virtual NO_DISCARD bool Trace(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT = 0;

	};
}