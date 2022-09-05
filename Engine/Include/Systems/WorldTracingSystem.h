#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/AccelerationStructure.h>
#include <Rendering/Native/SurfaceDescription.h>
#include <Rendering/Native/Vertex.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

class ALIGN(8) WorldTracingSystem final
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
	NO_DISCARD bool OcclusionRay(const Ray &ray, const float32 maximum_length) NOEXCEPT;

	/*
	*	Casts a ray into the world and checks for the hit distance. Returns if there was a hit.
	*/
	NO_DISCARD bool DistanceRay(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state = true) NOEXCEPT;

private:

	/*
	*	Vertex data class definition.
	*/
	class VertexData final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			DYNAMIC_MODEL,
			STATIC_MODEL
		};

		//The material resource.
		ResourcePointer<MaterialResource> _MaterialResource;

		//The normal.
		Vector3<float32> _Normal;

		//The tangent.
		Vector3<float32> _Tangent;

		//The texture coordinate.
		Vector2<float32> _TextureCoordinate;

		//The type.
		Type _Type;

	};

	//The acceleration structure.
	AccelerationStructure<VertexData> _AccelerationStructure;

	/*
	*	Casts a ray into the world and returns the radiance internally.
	*/
	NO_DISCARD Vector3<float32> RadianceRayInternal(const Ray &ray, const uint8 depth) NOEXCEPT;

	/*
	*	Casts an distance ray against models.
	*/
	NO_DISCARD bool DistanceRayModels(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state) NOEXCEPT;

	/*
	*	Casts a ray against the sky.
	*/
	NO_DISCARD Vector3<float32> SkyRay(const Ray &ray) NOEXCEPT;

};