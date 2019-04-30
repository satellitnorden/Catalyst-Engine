//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldRayTracingPipeline);

//World ray tracing pipeline constants.
namespace WorldRayTracingPipelineConstants
{
	constexpr int32 NUMBER_OF_ITERATIONS{ 1 };
	constexpr uint64 MAXIMUM_NUMBER_OF_MODELS{ 32 };
	constexpr uint64 MAXIMUM_NUMBER_OF_LIGHTS{ 4 };
}

/*
*	Light uniform data definition.
*/
class LightUniformData final
{

public:

	//The number of lights.
	int32 _NumberOfLights;

	//Some padding.
	Padding<12> _Padding;

	//The light data.
	StaticArray<LightComponent, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_LIGHTS> _LightData;

};

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The number of iterations.
	int32 _NumberOfIterations;

	//The current iteration.
	int32 _CurrentIteration;

	//The seeds.
	StaticArray<float, 7> _Seeds;

};

static_assert(offsetof(PushConstantData, _NumberOfIterations) == 0, "Oh no. ):");
static_assert(offsetof(PushConstantData, _CurrentIteration) == 4, "Oh no. ):");
static_assert(offsetof(PushConstantData, _Seeds) == 8, "Oh no. ):");

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
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RayClosestHit | ShaderStage::RayGeneration, 0, sizeof(PushConstantData));

	//Set the ray generation shader.
	SetRayGenerationShader(Shader::WorldRayGenerationShader);

	//Set the miss shader.
	SetMissShader(Shader::WorldRayMissShader);

	//Set the visibility miss shader.
	SetVisiblityMissShader(Shader::VisibilityRayMissShader);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::WorldRayClosestHitShader);
}

/*
*	Creates the render data table layout.
*/
void WorldRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 10> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration | ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayClosestHit | ShaderStage::RayMiss),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::StorageBuffer, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::StorageBuffer, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(9, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RayClosestHit)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void WorldRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::IndirectLighting));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectLighting));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
}

/*
*	Executes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Execute() NOEXCEPT
{
	static bool once{ false };

	if (!once)
	{
		const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT staticModelComponent{ ComponentManager::GetStaticModelStaticModelComponents() };
		const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticModelTransformComponents() };

		ASSERT(numberOfStaticModelComponents < WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, "Increase maximum number of models plz. c:");

		if (numberOfStaticModelComponents == 0)
		{
			SetIncludeInRender(false);

			return;
		}

		DynamicArray<TopLevelAccelerationStructureInstanceData> instances;
		instances.Reserve(numberOfStaticModelComponents);

		StaticArray<Material, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS> materials;

		for (uint64 i{ 0 }; i < numberOfStaticModelComponents; ++i, ++staticModelComponent, ++transformComponent)
		{
			instances.EmplaceFast(transformComponent->_LocalTransform, staticModelComponent->_Model->_AccelerationStructure, i);

			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(6, static_cast<uint32>(i), &_RenderDataTable, staticModelComponent->_Model->_VertexBuffer);
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(7, static_cast<uint32>(i), &_RenderDataTable, staticModelComponent->_Model->_IndexBuffer);

			materials[i] = staticModelComponent->_Material;
		}

		AccelerationStructureHandle handle;
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &handle);

		RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(4, 0, &_RenderDataTable, handle);
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(5, 0, &_RenderDataTable, ResourceLoader::GetTextureCube(HashString("Environment_TextureCube")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	
		{
			BufferHandle materialsBuffer;
			RenderingSystem::Instance->CreateBuffer(sizeof(Material) * WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &materialsBuffer);

			const void *const RESTRICT dataChunks[]{ materials.Data() };
			const uint64 dataSizes[]{ sizeof(Material) * WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS };

			RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &materialsBuffer);

			RenderingSystem::Instance->BindUniformBufferToRenderDataTable(8, 0, &_RenderDataTable, materialsBuffer);
		}

		{
			LightUniformData lightUniformData;

			BufferHandle lightsBuffer;
			RenderingSystem::Instance->CreateBuffer(sizeof(LightUniformData), BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &lightsBuffer);

			lightUniformData._NumberOfLights = static_cast<int32>(ComponentManager::GetNumberOfLightComponents());

			for (int32 i{ 0 }; i < lightUniformData._NumberOfLights; ++i)
			{
				lightUniformData._LightData[i] = ComponentManager::GetLightLightComponents()[i];
			}

			const void *const RESTRICT dataChunks[]{ &lightUniformData };
			const uint64 dataSizes[]{ sizeof(LightUniformData) };

			RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &lightsBuffer);

			RenderingSystem::Instance->BindUniformBufferToRenderDataTable(9, 0, &_RenderDataTable, lightsBuffer);
		}

		once = true;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	for (int32 i{ 0 }; i < WorldRayTracingPipelineConstants::NUMBER_OF_ITERATIONS; ++i)
	{
		//Push constants.
		PushConstantData data;

		data._NumberOfIterations = WorldRayTracingPipelineConstants::NUMBER_OF_ITERATIONS;
		data._CurrentIteration = i;

		for (float &seed : data._Seeds)
		{
			seed = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		}

		commandBuffer->PushConstants(this, ShaderStage::RayClosestHit | ShaderStage::RayGeneration, 0, sizeof(PushConstantData), &data);

		//Trace rays!
		commandBuffer->TraceRays(this);
	}
	
	//End the command buffer.
	commandBuffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}