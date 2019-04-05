//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Singleton definition.
DEFINE_SINGLETON(WorldRayTracingPipeline);

/*
*	Default constructor.
*/
WorldRayTracingPipeline::WorldRayTracingPipeline() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([]()
	{
		WorldRayTracingPipeline::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		WorldRayTracingPipeline::Instance->Execute();
	});
}

/*
*	Initializes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Initialize() NOEXCEPT
{
	//Set the ray generation shader.
	SetRayGenerationShader(Shader::None);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::None);

	//Set the miss shader shader.
	SetMissShader(Shader::None);
}

/*
*	Executes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Execute() NOEXCEPT
{

}