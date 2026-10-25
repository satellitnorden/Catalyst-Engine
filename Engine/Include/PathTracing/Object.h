#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

namespace PathTracing
{

	/*
	*	Object class.
	*	Represents a single object in a path tracing context.
	*	Should be overridden by child classes that implements the actual functionality.
	*/
	class Object
	{

	public:

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Object() NOEXCEPT
		{

		}

		/*
		*	Returns the bounding box of this object.
		*/
		virtual NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT = 0;

		/*
		*	Traces against this object.
		*	Should return if it hits, and if so, fill in the passed in result.
		*/
		virtual NO_DISCARD bool Trace(const Ray &ray) const NOEXCEPT = 0;

	};

}