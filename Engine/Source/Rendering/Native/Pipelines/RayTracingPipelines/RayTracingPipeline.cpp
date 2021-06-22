//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>

/*
*	Default constructor.
*/
RayTracingPipeline::RayTracingPipeline() NOEXCEPT
{
	//Set the type.
	SetType(Type::RayTracing);
}

/*
*	Resets this ray tracing pipeline.
*/
void RayTracingPipeline::ResetRayTracingPipeline() NOEXCEPT
{
	//Reset this pipeline.
	ResetPipeline();

	//Clear the hit groups.
	_HitGroups.Clear();

	//Clear the miss shaders.
	_MissShaders.Clear();
}