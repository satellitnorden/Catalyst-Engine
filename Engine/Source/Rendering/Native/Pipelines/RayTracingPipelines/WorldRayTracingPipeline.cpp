//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

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
	constexpr uint64 MAXIMUM_NUMBER_OF_MODELS{ 8 };
}

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

	//Set the ray generation shader.
	SetRayGenerationShader(Shader::WorldRayGenerationShader);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::WorldRayClosestHitShader);

	//Set the miss shader shader.
	SetMissShader(Shader::WorldRayMissShader);
}

/*
*	Creates the render data table layout.
*/
void WorldRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 6> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration | ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayClosestHit | ShaderStage::RayMiss),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageBuffer, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageBuffer, WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::UniformBuffer,1, ShaderStage::RayClosestHit)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void WorldRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));
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

			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(3, static_cast<uint32>(i), &_RenderDataTable, staticModelComponent->_Model->_VertexBuffer);
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(4, static_cast<uint32>(i), &_RenderDataTable, staticModelComponent->_Model->_IndexBuffer);

			materials[i] = staticModelComponent->_Material;
		}

		AccelerationStructureHandle handle;
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &handle);

		RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(1, 0, &_RenderDataTable, handle);
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, 0, &_RenderDataTable, ResourceLoader::GetTextureCube(HashString("Environment_TextureCube")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	
		BufferHandle materialsBuffer;
		RenderingSystem::Instance->CreateBuffer(sizeof(Material) * WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS, BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &materialsBuffer);
		
		const void *const RESTRICT dataChunks[]{ materials.Data() };
		const uint64 dataSizes[]{ sizeof(Material) * WorldRayTracingPipelineConstants::MAXIMUM_NUMBER_OF_MODELS };
		
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &materialsBuffer);

		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(5, 0, &_RenderDataTable, materialsBuffer);

		once = true;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Trace rays!
	commandBuffer->TraceRays(this);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}