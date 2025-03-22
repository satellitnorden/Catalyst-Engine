#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/System.h>

//Forward declarations.
class PathTracingAccelerationStructure;
class Ray;

class PathTracingSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		PathTracingSystem
	);

	/*
	*	Default constructor.
	*/
	PathTracingSystem() NOEXCEPT
	{

	}

	/*
	*	Starts path tracing.
	*/
	void Start() NOEXCEPT;

	/*
	*	Stops path tracing.
	*/
	void Stop(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Returns if path tracing is in progress.
	*/
	NO_DISCARD bool IsInProgress() const NOEXCEPT;

private:

	//Denotes if path tracing is in progress.
	bool _IsInProgress{ false };

	//The acceleration structure.
	PathTracingAccelerationStructure *RESTRICT _AccelerationStructure;

	//The intermediate texture.
	Texture2D<Vector4<float64>> _IntermediateTexture;

	//The final texture.
	Texture2D<Vector4<float32>> _FinalTexture;

	//The final texture handle.
	Texture2DHandle _FinalTextureHandle{ EMPTY_HANDLE };

	//The final texture index.
	uint32 _FinalTextureIndex{ 0 };

	//The tasks.
	DynamicArray<Task> _Tasks;

	//The queued rows.
	AtomicQueue<uint32, 4'096, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> _QueuedRows;

	//The current number of samples.
	uint64 _CurrentNumberOfSamples;

	//The current number of pixels.
	Atomic<uint64> _CurrentNumberOfPixels{ 0 };

	//The total number of pixels.
	uint64 _TotalNumberOfPixels;

	//The update identifier.
	uint64 _UpdateIdentifier;

	/*
	*	Updates the path tracing system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Executes the tasks.
	*/
	void ExecuteTasks() NOEXCEPT;

	/*
	*	Updates a task.
	*/
	void UpdateTask();

	/*
	*	Returns if all tasks are done.
	*/
	NO_DISCARD bool AllTasksDone() const NOEXCEPT;

	/*
	*	Casts a radiance ray.
	*/
	NO_DISCARD Vector3<float32> CastRadianceRay(const Ray &ray, const uint8 depth) NOEXCEPT;

	/*
	*	Generates an irradiance ray.
	*/
	void GenerateIrradianceRay
	(
		const Vector3<float32> &view_direction,
		const Vector3<float32> &normal,
		const float32 roughness,
		const float32 metallic,
		const float32 thickness,
		Vector3<float32> *const RESTRICT direction,
		float32 *const RESTRICT probability_density
	) NOEXCEPT;

};