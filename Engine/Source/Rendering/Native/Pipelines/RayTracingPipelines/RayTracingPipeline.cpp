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