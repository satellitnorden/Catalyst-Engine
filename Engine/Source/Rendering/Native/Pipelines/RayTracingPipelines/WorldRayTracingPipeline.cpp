//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

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
	SetRayGenerationShader(Shader::WorldRayGenerationShader);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::WorldRayClosestHitShader);

	//Set the miss shader shader.
	SetMissShader(Shader::WorldRayMissShader);
}

#include <Resources/Loading/ResourceLoader.h>

/*
*	Executes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Execute() NOEXCEPT
{
	static bool once{ false };

	//if (!once)
	{
		once = true;

		TopLevelAccelerationStructureInstanceData data{ MatrixConstants::IDENTITY, ResourceLoader::GetModel(HashString("Dresser_Model"))._AccelerationStructure };
		AccelerationStructureHandle handle;
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(data), &handle);
	}
}