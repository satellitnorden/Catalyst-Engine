#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

class WorldRayTracingPipeline final : public RayTracingPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldRayTracingPipeline);

	/*
	*	Default constructor.
	*/
	WorldRayTracingPipeline() NOEXCEPT;

private:

	/*
	*	Initializes this ray tracing pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this ray tracing pipeline.
	*/
	void Execute() NOEXCEPT;

};