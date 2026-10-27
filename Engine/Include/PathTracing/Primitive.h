#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Path tracing.
#include <PathTracing/Material.h>
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
		*	Default constructor.
		*/
		FORCE_INLINE Primitive(const PathTracing::Material *const RESTRICT material) NOEXCEPT
			:
			_Material(material)
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Primitive() NOEXCEPT
		{

		}

		/*
		*	Returns the material.
		*/
		FORCE_INLINE NO_DISCARD const PathTracing::Material *const RESTRICT GetMaterial() const NOEXCEPT
		{
			return _Material;
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
		*	Traces a surface ray against this primitive.
		*	Should return if it hits, and if so, fill in the surface information.
		*/
		virtual NO_DISCARD bool TraceSurface(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT = 0;

		/*
		*	Traces an occlusion ray against this primitive.
		*	Should return if it hits.
		*/
		virtual NO_DISCARD bool TraceOcclusion(const Ray &ray, const float32 maximum_distance) const NOEXCEPT = 0;

	private:

		//The material.
		const PathTracing::Material *RESTRICT _Material;

	};
}