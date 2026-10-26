#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Path tracing.
#include <PathTracing/AccelerationStructure.h>
#include <PathTracing/Material.h>

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
		*	Default constructor.
		*/
		FORCE_INLINE Object(const PathTracing::Material *const RESTRICT material) NOEXCEPT
			:
			_Material(material)
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Object() NOEXCEPT
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
		*	Returns the bounding box of this object.
		*/
		virtual NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT = 0;

		/*
		*	Adds the primitives for this object to the given acceleration structure.
		*/
		virtual void AddPrimitives(PathTracing::AccelerationStructure *const RESTRICT acceleration_structure) const NOEXCEPT = 0;

	private:

		//The material.
		const PathTracing::Material *RESTRICT _Material;

	};

}