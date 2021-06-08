#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/SurfaceDescription.h>
#include <Rendering/Native/Vertex.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

class WorldTracingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldTracingSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE WorldTracingSystem() NOEXCEPT
	{

	}

	/*
	*	Caches the world state.
	*/
	void CacheWorldState() NOEXCEPT;

	/*
	*	Casts a ray into the world and returns the radiance.
	*/
	NO_DISCARD Vector3<float32> RadianceRay(const Ray &ray) NOEXCEPT;

	/*
	*	Casts a ray into the world and returns if there was occlusion.
	*/
	NO_DISCARD bool OcclusionRay(const Ray &ray) NOEXCEPT;

private:

	/*
	*	Cached model state class definition.
	*/
	class CachedModelState final
	{

	public:

		/*
		*	Triangle class definition.
		*/
		class Triangle final
		{

		public:

			//The material resources.
			ResourcePointer<MaterialResource> _MaterialResource;

			//The vertices.
			StaticArray<Vertex, 3> _Vertices;

		};

		//The world space axis aligned bounding box.
		AxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

		//The triangles.
		DynamicArray<Triangle> _Triangles;

	};

	//The cached model state.
	DynamicArray<CachedModelState> _CachedModelState;

	/*
	*	Caches the world model state.
	*/
	void CacheWorldModelState() NOEXCEPT;

	/*
	*	Casts a ray into the world and returns the radiance internally.
	*/
	NO_DISCARD Vector3<float32> RadianceRayInternal(const Ray &ray, const uint8 depth) NOEXCEPT;

	/*
	*	Casts a surface ray against models.
	*/
	NO_DISCARD bool SurfaceRayModels(const Ray &ray, float32 *closest_intersection_distance, SurfaceDescription *const RESTRICT surface_description) NOEXCEPT;

	/*
	*	Casts an occlusion ray against models.
	*/
	NO_DISCARD bool OcclusionRayModels(const Ray &ray) NOEXCEPT;

};