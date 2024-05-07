#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/AccelerationStructure.h>
#include <Rendering/Native/SurfaceDescription.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/System.h>

class WorldTracingSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		WorldTracingSystem
	);

	/*
	*	Radiance ray parameters class definition.
	*/
	class RadianceRayParameters final
	{

	public:

		//Denotes whether or not to do improved ray generation.
		bool _ImprovedRayGeneration;

	};

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
	*	Returns a description of the surface which the given ray hits. Returns false if no surface was hit.
	*/
	NO_DISCARD bool SurfaceDescriptionRay(const Ray &ray, float32 *const RESTRICT intersection_distance, SurfaceDescription *const RESTRICT surface_description) NOEXCEPT;

	/*
	*	Casts a ray into the world and returns the radiance.
	*/
	NO_DISCARD Vector3<float32> RadianceRay(const Ray &ray, const RadianceRayParameters *const RESTRICT parameters) NOEXCEPT;

	/*
	*	Casts a ray into the world and returns if there was occlusion.
	*/
	NO_DISCARD bool OcclusionRay(const Ray &ray, const float32 maximum_length) NOEXCEPT;

	/*
	*	Casts a ray into the world and checks for the hit distance. Returns if there was a hit.
	*/
	NO_DISCARD bool DistanceRay(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state = true) NOEXCEPT;

	/*
	*	Casts a ray against the sky.
	*/
	NO_DISCARD Vector3<float32> SkyRay(const Ray &ray) NOEXCEPT;

private:

	//Friend declarations.
	friend class DiscardFunctions;

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

		//The material.
		AssetPointer<MaterialAsset> _Material;

		//The normal.
		Vector3<float32> _Normal;

		//The tangent.
		Vector3<float32> _Tangent;

		//The texture coordinate.
		Vector2<float32> _TextureCoordinate;

	};

	//The acceleration structure.
	AccelerationStructure<VertexData> _AccelerationStructure;

	/*
	*	Casts a ray into the world and returns the radiance internally.
	*/
	NO_DISCARD Vector3<float32> RadianceRayInternal(const Ray &ray, const RadianceRayParameters *const RESTRICT parameters, const uint8 depth) NOEXCEPT;

	/*
	*	Casts an distance ray against models.
	*/
	NO_DISCARD bool DistanceRayModels(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state) NOEXCEPT;

	/*
	*	The probability density function.
	*/
	NO_DISCARD float32 ProbabilityDensityFunction
	(
		const Vector3<float32> &view_direction,
		const Vector3<float32> &normal,
		const float32 roughness,
		const float32 metallic,
		const Vector3<float32> &direction
	) NOEXCEPT;

	/*
	*	Generates an irradiance ray.
	*/
	void GenerateIrradianceRay
	(
		const Vector3<float32> &view_direction,
		const Vector3<float32> &normal,
		const float32 roughness,
		const float32 metallic,
		Vector3<float32> *const RESTRICT direction,
		float32 *const RESTRICT probability_density,
		const bool improved_ray_generation
	) NOEXCEPT;

	/*
	*	Returns the ambient occlusion at the given position.
	*/
	NO_DISCARD float32 SampleAmbientOcclusion(const Vector3<float32> &position, const Vector3<float32> &normal) NOEXCEPT;

};