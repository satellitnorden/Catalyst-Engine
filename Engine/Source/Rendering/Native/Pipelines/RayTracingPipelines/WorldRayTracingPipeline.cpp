//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The noise offset.
	Vector2<int32> _NoiseOffset;

};

/*
*	Initializes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Create the noise textures.
	CreateNoiseTextures();

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataComputeRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetModelSystem()->GetModelDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RayGeneration, 0, sizeof(PushConstantData));

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
*	Executes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Execute() NOEXCEPT
{
	static bool once{ false };

	if (!once)
	{
		//Create the top level acceleration structure. (:
		const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT staticModelComponent{ ComponentManager::GetStaticModelStaticModelComponents() };
		const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticModelTransformComponents() };

		ASSERT(numberOfStaticModelComponents < RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, "Increase maximum number of models plz. c:");

		if (numberOfStaticModelComponents == 0)
		{
			SetIncludeInRender(false);

			return;
		}

		DynamicArray<TopLevelAccelerationStructureInstanceData> instances;
		instances.Reserve(numberOfStaticModelComponents);

		for (uint64 i{ 0 }; i < numberOfStaticModelComponents; ++i, ++staticModelComponent, ++transformComponent)
		{
			instances.EmplaceFast(transformComponent->_LocalTransform, staticModelComponent->_Model->_AccelerationStructure, i);
		}

		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_TopLevelAccelerationStructure);

		once = true;
	}

	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SpecularIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(5, 0, &currentRenderDataTable, _NoiseTextures[_CurrentNoiseTextureIndex], RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat));
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(6, 0, &currentRenderDataTable, _TopLevelAccelerationStructure);
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(7, 0, &currentRenderDataTable, ResourceLoader::GetTextureCube(HashString("Environment_TextureCube")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, currentRenderDataTable);
	commandBuffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataComputeRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 3, RenderingSystem::Instance->GetModelSystem()->GetCurrentModelDataRenderDataTable());

	//Push constants.
	PushConstantData data;

	data._NoiseOffset._X = CatalystRandomMath::RandomIntegerInRange<int32>(0, 32);
	data._NoiseOffset._Y = CatalystRandomMath::RandomIntegerInRange<int32>(0, 32);

	commandBuffer->PushConstants(this, ShaderStage::RayGeneration, 0, sizeof(PushConstantData), &data);

	//Trace rays!
	commandBuffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in render.
	SetIncludeInRender(true);

	//Update the current texture index.
	_CurrentNoiseTextureIndex = _CurrentNoiseTextureIndex == NUMBER_OF_NOISE_TEXTURES - 1 ? 0 : _CurrentNoiseTextureIndex + 1;
}

/*
*	Creates the render data table layout.
*/
void WorldRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 8> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration | ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayClosestHit | ShaderStage::RayMiss)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void WorldRayTracingPipeline::CreateRenderDataTables() NOEXCEPT
{
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Creates the noise textures.
*/
void WorldRayTracingPipeline::CreateNoiseTextures() NOEXCEPT
{
	//Create all the noise textures.
	for (uint8 i{ 0 }; i < NUMBER_OF_NOISE_TEXTURES; ++i)
	{
		Texture2D<Vector4<byte>> noiseTexture{ NOISE_TEXTURE_SIZE };

		for (uint8 x{ 0 }; x < NOISE_TEXTURE_SIZE; ++x)
		{
			for (uint8 y{ 0 }; y < NOISE_TEXTURE_SIZE; ++y)
			{
				Vector4<byte> &noise{ noiseTexture.At(x, y) };

				noise._X = CatalystRandomMath::RandomIntegerInRange<byte>(0, 255);
				noise._Y = CatalystRandomMath::RandomIntegerInRange<byte>(0, 255);
				noise._Z = CatalystRandomMath::RandomIntegerInRange<byte>(0, 255);
				noise._W = CatalystRandomMath::RandomIntegerInRange<byte>(0, 255);
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(noiseTexture), TextureFormat::R8G8B8A8_Byte), &_NoiseTextures[i]);
	}
}