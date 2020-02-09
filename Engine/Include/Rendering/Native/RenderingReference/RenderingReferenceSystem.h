#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/RenderingReference/SurfaceDescription.h>
#include <Rendering/Native/RenderingReference/VolumetricDescription.h>

//User interface.
#include <UserInterface/TextUserInterfaceElement.h>

class RenderingReferenceSystem final
{

public:

	/*
	*	Updates the rendering reference system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Returns whether or not rendering reference is in progress.
	*/
	FORCE_INLINE NO_DISCARD bool IsRenderingReferenceInProgress() const NOEXCEPT
	{
		return _RenderingReferenceInProgress;
	}

private:

	/*
	*	Asynchronous data class definition.
	*/
	class AsynchronousData final
	{

	public:

		//The task.
		Task _Task;

		//The start Y coordinate.
		uint32 _StartY;

		//The end Y coordinate.
		uint32 _EndY;

	};

	//Denotes whether or not rendering reference is in progress.
	bool _RenderingReferenceInProgress{ false };

	//The rendering reference texture.
	Texture2D<Vector4<float>> _RenderingReferenceTexture;

	//The rendering reference texture handle.
	Texture2DHandle _RenderingReferenceTextureHandle{ EMPTY_HANDLE };

	//The rendering reference texture index.
	uint32 _RenderingReferenceTextureIndex;

	//The number of iterations.
	uint32 _Iterations{ 0 };

	//Container for all the asynchronous data.
	DynamicArray<AsynchronousData> _AsynchronousData;

	//The progress information.
	TextUserInterfaceElement *RESTRICT _ProgressInformation;

	//Keeps track of the number of texels calculated.
	std::atomic<uint64> _TexelsCalculated;

	/*
	*	Starts the rendering reference.
	*/
	void StartRenderingReference() NOEXCEPT;

	/*
	*	Ends the rendering reference.
	*/
	void EndRenderingReference() NOEXCEPT;

	/*
	*	Updates the rendering reference.
	*/
	void UpdateRenderingReference() NOEXCEPT;

	/*
	*	Executes asynchronously.
	*/
	void ExecuteAsynchronous(const AsynchronousData *const RESTRICT data) NOEXCEPT;

	/*
	*	Calculates a texel.
	*/
	void CalculateTexel(const uint32 X, const uint32 Y) NOEXCEPT;

	/*
	*	Calculates the ray direction.
	*/
	Vector3<float> CalculateRayDirection(const uint32 X, const uint32 Y) NOEXCEPT;

	/*
	*	Casts a ray against the scene. Returns the color.
	*/
	NO_DISCARD Vector3<float> CastRayScene(const Ray &ray, const uint8 recursion) NOEXCEPT;

	/*
	*	Casts a surface ray against the volumetric particles. Returns if there was a hit.
	*/
	NO_DISCARD bool CastVolumetricRayScene(const Ray &ray, VolumetricDescription *const RESTRICT volumetric_description, float *const RESTRICT hit_distance) NOEXCEPT;

	/*
	*	Casts a surface ray against terrain. Returns if there was a hit.
	*/
	NO_DISCARD bool CastSurfaceRayTerrain(const Ray &ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT;

	/*
	*	Casts a ray against the sky. Returns the color.
	*/
	NO_DISCARD Vector3<float> CastRaySky(const Ray &ray) NOEXCEPT;

	/*
	*	Calculates the surface lighting.
	*/
	NO_DISCARD Vector3<float> CalculateSurfaceLighting(const Ray &incoming_ray, const float hit_distance, const SurfaceDescription &surface_description, const uint8 recursion) NOEXCEPT;

	/*
	*	Calculates the volumetric lighting.
	*/
	NO_DISCARD Vector3<float> CalculateVolumetricLighting(const Ray& incoming_ray, const float hit_distance, const VolumetricDescription &volumetric_description, const uint8 recursion) NOEXCEPT;

	/*
	*	Casts a shadow ray against the scene. Returns if there was a hit.
	*/
	NO_DISCARD bool CastShadowRayScene(const Ray &ray) NOEXCEPT;

	/*
	*	Casts a shadow ray against the terrain. Returns if there was a hit.
	*/
	NO_DISCARD bool CastShadowRayTerrain(const Ray& ray) NOEXCEPT;

};
#endif