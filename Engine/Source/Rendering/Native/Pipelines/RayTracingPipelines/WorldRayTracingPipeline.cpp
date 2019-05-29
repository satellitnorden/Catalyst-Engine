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
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
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
			instances.EmplaceFast(transformComponent->_WorldTransform, staticModelComponent->_Model->_BottomLevelAccelerationStructure, i);
		}

		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_TopLevelAccelerationStructure);

		once = true;
	}

	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SpecularIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectLighting));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(5, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(6, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(7, 0, &currentRenderDataTable, _NoiseTextures[_CurrentNoiseTextureIndex], RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat));
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(8, 0, &currentRenderDataTable, _TopLevelAccelerationStructure);
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(9, 0, &currentRenderDataTable, ResourceLoader::GetTextureCube(HashString("Environment_TextureCube")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, currentRenderDataTable);
	commandBuffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 3, RenderingSystem::Instance->GetModelSystem()->GetCurrentModelDataRenderDataTable());

	//Push constants.
	PushConstantData data;

	data._NoiseOffset._X = CatalystRandomMath::RandomIntegerInRange<int32>(0, 64);
	data._NoiseOffset._Y = CatalystRandomMath::RandomIntegerInRange<int32>(0, 64);

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
	StaticArray<RenderDataTableLayoutBinding, 10> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration | ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(9, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayClosestHit | ShaderStage::RayMiss)
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
#define USE_BLUE_NOISE_TEXTURES true

#if USE_BLUE_NOISE_TEXTURES
	ResourceLoader::LoadResourceCollection("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\BlueNoiseResourceCollection.crc");

	_NoiseTextures[0] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_0_Texture2D"))._Texture2D;
	_NoiseTextures[1] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_1_Texture2D"))._Texture2D;
	_NoiseTextures[2] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_2_Texture2D"))._Texture2D;
	_NoiseTextures[3] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_3_Texture2D"))._Texture2D;
	_NoiseTextures[4] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_4_Texture2D"))._Texture2D;
	_NoiseTextures[5] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_5_Texture2D"))._Texture2D;
	_NoiseTextures[6] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_6_Texture2D"))._Texture2D;
	_NoiseTextures[7] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_7_Texture2D"))._Texture2D;
	_NoiseTextures[8] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_8_Texture2D"))._Texture2D;
	_NoiseTextures[9] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_9_Texture2D"))._Texture2D;
	_NoiseTextures[10] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_10_Texture2D"))._Texture2D;
	_NoiseTextures[11] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_11_Texture2D"))._Texture2D;
	_NoiseTextures[12] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_12_Texture2D"))._Texture2D;
	_NoiseTextures[13] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_13_Texture2D"))._Texture2D;
	_NoiseTextures[14] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_14_Texture2D"))._Texture2D;
	_NoiseTextures[15] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_15_Texture2D"))._Texture2D;
	_NoiseTextures[16] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_16_Texture2D"))._Texture2D;
	_NoiseTextures[17] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_17_Texture2D"))._Texture2D;
	_NoiseTextures[18] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_18_Texture2D"))._Texture2D;
	_NoiseTextures[19] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_19_Texture2D"))._Texture2D;
	_NoiseTextures[20] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_20_Texture2D"))._Texture2D;
	_NoiseTextures[21] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_21_Texture2D"))._Texture2D;
	_NoiseTextures[22] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_22_Texture2D"))._Texture2D;
	_NoiseTextures[23] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_23_Texture2D"))._Texture2D;
	_NoiseTextures[24] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_24_Texture2D"))._Texture2D;
	_NoiseTextures[25] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_25_Texture2D"))._Texture2D;
	_NoiseTextures[26] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_26_Texture2D"))._Texture2D;
	_NoiseTextures[27] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_27_Texture2D"))._Texture2D;
	_NoiseTextures[28] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_28_Texture2D"))._Texture2D;
	_NoiseTextures[29] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_29_Texture2D"))._Texture2D;
	_NoiseTextures[30] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_30_Texture2D"))._Texture2D;
	_NoiseTextures[31] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_31_Texture2D"))._Texture2D;
	_NoiseTextures[32] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_32_Texture2D"))._Texture2D;
	_NoiseTextures[33] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_33_Texture2D"))._Texture2D;
	_NoiseTextures[34] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_34_Texture2D"))._Texture2D;
	_NoiseTextures[35] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_35_Texture2D"))._Texture2D;
	_NoiseTextures[36] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_36_Texture2D"))._Texture2D;
	_NoiseTextures[37] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_37_Texture2D"))._Texture2D;
	_NoiseTextures[38] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_38_Texture2D"))._Texture2D;
	_NoiseTextures[39] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_39_Texture2D"))._Texture2D;
	_NoiseTextures[40] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_40_Texture2D"))._Texture2D;
	_NoiseTextures[41] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_41_Texture2D"))._Texture2D;
	_NoiseTextures[42] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_42_Texture2D"))._Texture2D;
	_NoiseTextures[43] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_43_Texture2D"))._Texture2D;
	_NoiseTextures[44] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_44_Texture2D"))._Texture2D;
	_NoiseTextures[45] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_45_Texture2D"))._Texture2D;
	_NoiseTextures[46] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_46_Texture2D"))._Texture2D;
	_NoiseTextures[47] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_47_Texture2D"))._Texture2D;
	_NoiseTextures[48] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_48_Texture2D"))._Texture2D;
	_NoiseTextures[49] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_49_Texture2D"))._Texture2D;
	_NoiseTextures[50] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_50_Texture2D"))._Texture2D;
	_NoiseTextures[51] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_51_Texture2D"))._Texture2D;
	_NoiseTextures[52] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_52_Texture2D"))._Texture2D;
	_NoiseTextures[53] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_53_Texture2D"))._Texture2D;
	_NoiseTextures[54] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_54_Texture2D"))._Texture2D;
	_NoiseTextures[55] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_55_Texture2D"))._Texture2D;
	_NoiseTextures[56] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_56_Texture2D"))._Texture2D;
	_NoiseTextures[57] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_57_Texture2D"))._Texture2D;
	_NoiseTextures[58] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_58_Texture2D"))._Texture2D;
	_NoiseTextures[59] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_59_Texture2D"))._Texture2D;
	_NoiseTextures[60] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_60_Texture2D"))._Texture2D;
	_NoiseTextures[61] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_61_Texture2D"))._Texture2D;
	_NoiseTextures[62] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_62_Texture2D"))._Texture2D;
	_NoiseTextures[63] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_63_Texture2D"))._Texture2D;
#else
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
#endif
}