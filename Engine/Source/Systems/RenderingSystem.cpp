//Header file.
#include <Systems/RenderingSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/TerrainEntity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystMath.h>

//Rendering.
#include <Rendering/Engine/CommonPhysicalModelData.h>
#include <Rendering/Engine/OceanMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/VegetationMaterial.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/TextureData.h>
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/VegetationMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Define the current rendering system.
#if defined(CATALYST_WINDOWS) || defined(CATALYST_ANDROID)
	#define CURRENT_RENDERING_SYSTEM VulkanRenderingSystem
#endif

/*
*	Initializes the rendering system.
*/
void RenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeSystem();

	//Register all render passes.
	RegisterRenderPasses();

	//Initialize all render passes.
	InitializeRenderPasses();

	//Initialize the common particle materials.
	InitializeCommonParticleMaterials();

	//Initializes the common physical materials.
	InitializeCommonPhysicalMaterials();

	//Initialize the common physical models.
	InitializeCommonPhysicalModels();

	//Initialize all default assets.
	InitializeDefaultAssets();
}

/*
*	Pre-updates the rendering system synchronously.
*/
void RenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Update the matrices.
	UpdateMatrices();
}

/*
*	Updates the rendering system synchronously.
*/
void RenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Pre-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PreUpdateSystemSynchronous();

	//Render all render passes.
	for (RenderPass *const RESTRICT renderPass : renderPasses)
	{
		renderPass->RenderAsynchronous();
	}

	//Post-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PostUpdateSystemSynchronous();
}

/*
*	Releases the rendering system.
*/
void RenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->ReleaseSystem();
}

/*
*	Returns the resolution.
*/
Resolution RenderingSystem::GetResolution() const NOEXCEPT
{
	//Return the render resolution via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetResolution();
}

/*
*	Returns the current frame index.
*/
uint8 RenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	//Return the current frame index via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentFrameIndex();
}

/*
*	Creates and returns a texture 2D given the texture data.
*/
Texture2DHandle RenderingSystem::CreateTexture2D(const TextureData &textureData) const NOEXCEPT
{
	//Create the texture 2D via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->Create2DTexture(textureData);
}

/*
*	Destroys a texture 2D
*/
void RenderingSystem::DestroyTexture2D(Texture2DHandle texture) const NOEXCEPT
{
	//Destroy the texture 2D via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyTexture2D(texture);
}

/*
*	Destroys a render data table.
*/
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle renderDataTable) const NOEXCEPT
{
	//Destroy the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyRenderDataTable(renderDataTable);
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle RenderingSystem::CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	//Create the constant buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateConstantBuffer(data, dataSizes, dataChunks);
}

/*
*	Finalizes the initialization of a render pass.
*/
void RenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Finalize the initialization of this render pass via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->FinalizeRenderPassInitialization(renderPass);
}

/*
*	Returns the current dynamic uniform data descriptor set.
*/
RenderDataTableHandle RenderingSystem::GetCurrentDynamicUniformDataDescriptorSet() const NOEXCEPT
{
	//Return the current dynamic uniform data descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentDynamicUniformDataDescriptorSet();
}

/*
*	Returns the current environment data descriptor set.
*/
RenderDataTableHandle RenderingSystem::GetCurrentEnvironmentDataDescriptorSet() const NOEXCEPT
{
	//Return the current environment data descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentEnvironmentDataDescriptorSet();
}

/*
*	Returns the current ocean descriptor set.
*/
RenderDataTableHandle RenderingSystem::GetCurrentOceanDescriptorSet() const NOEXCEPT
{
	//Return the current ocean descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentOceanDescriptorSet();
}

/*
*	Returns the given render data table.
*/
RenderDataTableHandle RenderingSystem::GetRenderDataTable(const RenderDataTable renderDataTable) const NOEXCEPT
{
	//Return the given render data table via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetRenderDataTable(renderDataTable);
}

/*
*	Creates an environment material.
*/
void RenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the environment material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateEnvironmentMaterial(environmentMaterialData, environmentMaterial);
}

/*
*	Creates an ocean material.
*/
void RenderingSystem::CreateOceanMaterial(const WaterMaterialData &waterMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT
{
	//Create the normal map texture.
	oceanMaterial.normalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(waterMaterialData.normalMapData, waterMaterialData.width, waterMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a physical model.
*/
void RenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ physicalModelData.vertices.Data(), physicalModelData.indices.Data() };
	const VkDeviceSize modelDataSizes[]{ sizeof(PhysicalVertex) * physicalModelData.vertices.Size(), sizeof(uint32) * physicalModelData.indices.Size() };
	ConstantBufferHandle buffer = CreateConstantBuffer(modelData, modelDataSizes, 2);

	//Set up the physical model.
	physicalModel.GetAxisAlignedBoundingBox().minimum = Vector3(-physicalModelData.extent, -physicalModelData.extent, -physicalModelData.extent);
	physicalModel.GetAxisAlignedBoundingBox().maximum = Vector3(physicalModelData.extent, physicalModelData.extent, physicalModelData.extent);
	physicalModel.SetBuffer(buffer);
	physicalModel.SetIndexOffset(modelDataSizes[0]);
	physicalModel.SetIndexCount(static_cast<uint32>(physicalModelData.indices.Size()));
}

/*
*	Creates a particle material.
*/
void RenderingSystem::CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT
{
	//Create the albedo texture
	particleMaterial.albedoTexture = CreateTexture2D(TextureData(TextureDataContainer(particleMaterialData.albedoData, particleMaterialData.width, particleMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Create the albedo texture.
	physicalMaterial.albedoTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData.albedoData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	physicalMaterial.normalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData.normalMapData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the material properties texture.
	physicalMaterial.materialPropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData.materialPropertiesData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the first layer albedo.
	terrainMaterial.firstLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.firstLayerAlbedoData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer normal map.
	terrainMaterial.firstLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.firstLayerNormalMapData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer material properties.
	terrainMaterial.firstLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.firstLayerMaterialPropertiesData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer albedo.
	terrainMaterial.secondLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.secondLayerAlbedoData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer normal map.
	terrainMaterial.secondLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.secondLayerNormalMapData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer material properties.
	terrainMaterial.secondLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.secondLayerMaterialPropertiesData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer albedo.
	terrainMaterial.thirdLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerAlbedoData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer normal map.
	terrainMaterial.thirdLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerNormalMapData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer material properties.
	terrainMaterial.thirdLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerMaterialPropertiesData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer albedo.
	terrainMaterial.fourthLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerAlbedoData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer normal map.
	terrainMaterial.fourthLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerNormalMapData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer material properties.
	terrainMaterial.fourthLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerMaterialPropertiesData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer albedo.
	terrainMaterial.fifthLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerAlbedoData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer normal map.
	terrainMaterial.fifthLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerNormalMapData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer material properties.
	terrainMaterial.fifthLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerMaterialPropertiesData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a vegetation material.
*/
void RenderingSystem::CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT
{
	//Create the mask texture.
	vegetationMaterial.maskTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData.maskData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the albedo texture.
	vegetationMaterial.albedoTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData.albedoData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	vegetationMaterial.normalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData.normalMapData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the properties texture.
	vegetationMaterial.propertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData.propertiesData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Initializes a terrain entity.
*/
void RenderingSystem::InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Initialize the terrain entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeTerrainEntity(entity, data);
}

/*
*	Terminates a terrain entity.
*/
void RenderingSystem::TerminateTerrainEntity(const TerrainEntity *const RESTRICT entity) const NOEXCEPT
{
	//Retrieve the component.
	TerrainComponent &component{ ComponentManager::GetTerrainComponents()[entity->GetComponentsIndex()] };
	TerrainRenderComponent &renderComponent{ ComponentManager::GetTerrainRenderComponents()[entity->GetComponentsIndex()] };

	//Destroy the terrain entity's resources.
	DestroyTexture2D(component.terrainPropertiesTexture);
	DestroyRenderDataTable(renderComponent.renderDataTable);
}

/*
*	Initializes a static physical entity.
*/
void RenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Initialize the static physical entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeStaticPhysicalEntity(staticPhysicalEntity, model, position, rotation, scale);
}

/*
*	Initializes an instanced physical entity.
*/
void RenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Initialize the instanced physical entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeInstancedPhysicalEntity(entity, model, transformations);
}

/*
*	Initializes a vegetation entity.
*/
void RenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Initialize the vegetation entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeVegetationEntity(entity, material, transformations, properties);
}

/*
*	Initializes a particle system entity.
*/
void RenderingSystem::InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT
{
	//Initialize the particle system entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeParticleSystemEntity(entity, material, properties);
}

/*
*	Creates and returns a uniform buffer.
*/
UniformBufferHandle RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	//Create the uniform buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Registers all render passes.
*/
void RenderingSystem::RegisterRenderPasses() NOEXCEPT
{
	//Register all render passes.
	renderPasses[INDEX(RenderPassSubStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::DirectionalStaticPhysicalShadow)] = DirectionalStaticPhysicalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::DirectionalInstancedPhysicalShadow)] = DirectionalInstancedPhysicalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Terrain)] = TerrainRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::StaticPhysical)] = StaticPhysicalRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::InstancedPhysical)] = InstancedPhysicalRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Vegetation)] = VegetationRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::SceenSpaceAmbientOcclusion)] = ScreenSpaceAmbientOcclusionRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::SceenSpaceAmbientOcclusionHorizontalBlur)] = ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::SceenSpaceAmbientOcclusionVerticalBlur)] = ScreenSpaceAmbientOcclusionVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Lighting)] = LightingRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Sky)] = SkyRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Ocean)] = OceanRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::Bloom)] = BloomRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::BloomHorizontalBlur)] = BloomHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::BloomVerticalBlur)] = BloomVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::PostProcessingBloom)] = PostProcessingBloomRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::PostProcessingHorizontalBlur)] = PostProcessingHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::PostProcessingVerticalBlur)] = PostProcessingVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassSubStage::PostProcessing)] = PostProcessingRenderPass::Instance.Get();
}

/*
*	Initializes all render passes.
*/
void RenderingSystem::InitializeRenderPasses() NOEXCEPT
{
	//Initialize all render passes.
	for (RenderPass *const RESTRICT renderPass : renderPasses)
	{
		renderPass->InitializeAsynchronous();
	}

	//Wait for all render passes to finish initialization.
	for (RenderPass *const RESTRICT renderPass : renderPasses)
	{
		renderPass->WaitForInitialization();
	}
}

/*
*	Initializes the common particle materials.
*/
void RenderingSystem::InitializeCommonParticleMaterials() NOEXCEPT
{
	{
		//Create the white circle common particle material.
		constexpr uint8 RESOLUTION{ 7 };
		constexpr Vector2 MIDDLE_POINT{ 0.5f, 0.5f };

		ParticleMaterialData data;

		data.mipmapLevels = 1;
		data.width = RESOLUTION;
		data.height = RESOLUTION;

		data.albedoData.UpsizeSlow(1);
		data.albedoData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint8 i = 0; i < RESOLUTION; ++i)
		{
			for (uint8 j = 0; j < RESOLUTION; ++j)
			{
				const Vector2 point{ static_cast<float>(i) / static_cast<float>(RESOLUTION - 1), static_cast<float>(j) / static_cast<float>(RESOLUTION - 1) };
				const float distance{ Vector2::LengthSquared(MIDDLE_POINT - point) * 2.0f };
				float alpha{ 1.0f - distance };
				alpha *= alpha;
				alpha *= alpha;

				data.albedoData[0].EmplaceFast(255);
				data.albedoData[0].EmplaceFast(255);
				data.albedoData[0].EmplaceFast(255);
				data.albedoData[0].EmplaceFast(static_cast<byte>(static_cast<float>(255) * alpha));
			}
		}

		CreateParticleMaterial(data, commonParticleMaterials[INDEX(CommonParticleMaterial::WhiteCircle)]);
	}
}

/*
*	Initializes the common physical materials.
*/
void RenderingSystem::InitializeCommonPhysicalMaterials() NOEXCEPT
{
	{
		//Create the purple common physical material.
		PhysicalMaterialData physicalMaterialData;
		
		physicalMaterialData.mipmapLevels = 1;
		physicalMaterialData.width = 1;
		physicalMaterialData.height = 1;
		physicalMaterialData.albedoData.UpsizeSlow(1);
		physicalMaterialData.albedoData[0].Reserve(4);
		physicalMaterialData.albedoData[0].EmplaceFast(255);
		physicalMaterialData.albedoData[0].EmplaceFast(0);
		physicalMaterialData.albedoData[0].EmplaceFast(0);
		physicalMaterialData.albedoData[0].EmplaceFast(255);
		physicalMaterialData.normalMapData.UpsizeSlow(1);
		physicalMaterialData.normalMapData[0].Reserve(4);
		physicalMaterialData.normalMapData[0].EmplaceFast(127);
		physicalMaterialData.normalMapData[0].EmplaceFast(127);
		physicalMaterialData.normalMapData[0].EmplaceFast(255);
		physicalMaterialData.normalMapData[0].EmplaceFast(255);
		physicalMaterialData.materialPropertiesData.UpsizeSlow(1);
		physicalMaterialData.materialPropertiesData[0].Reserve(4);
		physicalMaterialData.materialPropertiesData[0].EmplaceFast(127);
		physicalMaterialData.materialPropertiesData[0].EmplaceFast(255);
		physicalMaterialData.materialPropertiesData[0].EmplaceFast(255);
		physicalMaterialData.materialPropertiesData[0].EmplaceFast(0);

		CreatePhysicalMaterial(physicalMaterialData, commonPhysicalMaterials[INDEX(CommonPhysicalMaterial::Red)]);
	}
}

/*
*	Initializes the common physical models.
*/
void RenderingSystem::InitializeCommonPhysicalModels() NOEXCEPT
{
	{
		//Create the cube common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetCubePhysicalModelData(data);
		CreatePhysicalModel(data, commonPhysicalModels[INDEX(CommonPhysicalModel::Cube)]);
	}

	{
		//Create the octahedron common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetOctahedronPhysicalModelData(data);
		CreatePhysicalModel(data, commonPhysicalModels[INDEX(CommonPhysicalModel::Octahedron)]);
	}

	{
		//Create the plane common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetPlanePhysicalModelData(data);
		CreatePhysicalModel(data, commonPhysicalModels[INDEX(CommonPhysicalModel::Plane)]);
	}
}

/*
*	Initializes all default assets.
*/
void RenderingSystem::InitializeDefaultAssets() NOEXCEPT
{
	{
		//Initialize the default night environment material.
		constexpr Vector3 BOTTOM_COLOR{ 0.025f, 0.025f, 0.025f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f / 64.0f, 1.0f / 64.0f };

		EnvironmentMaterialData data;

		data.diffuseResolution = 2;
		data.diffuseData.Reserve(16 * 6);
		data.diffuseIrradianceResolution = 2;
		data.diffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}
		}

		CreateEnvironmentMaterial(data, defaultNightEnvironmentMaterial);

		EnvironmentManager::Instance->SetNightEnvironmentMaterial(defaultNightEnvironmentMaterial);
	}

	{
		//Initialize the default day environment material.
		constexpr Vector3 BOTTOM_COLOR{ 0.25f, 0.25f, 0.25f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f, 1.0f };

		EnvironmentMaterialData data;

		data.diffuseResolution = 2;
		data.diffuseData.Reserve(16 * 6);
		data.diffuseIrradianceResolution = 2;
		data.diffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(TOP_COLOR.X);
				data.diffuseData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(TOP_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);

				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.X);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Y);
				data.diffuseIrradianceData.EmplaceFast(BOTTOM_COLOR.Z);
				data.diffuseIrradianceData.EmplaceFast(1.0f);
			}
		}

		CreateEnvironmentMaterial(data, defaultDayEnvironmentMaterial);

		EnvironmentManager::Instance->SetDayEnvironmentMaterial(defaultDayEnvironmentMaterial);
	}

	//Set the environment blend to be day by default.
	EnvironmentManager::Instance->SetEnvironmentBlend(1.0f);

	{
		//Initialize the default ocean material.
		constexpr uint8 RESOLUTION{ 32 };

		WaterMaterialData data;

		data.mipmapLevels = 1;
		data.width = RESOLUTION;
		data.height = RESOLUTION;
		data.normalMapData.UpsizeSlow(1);
		data.normalMapData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint16 i = 0; i < RESOLUTION * RESOLUTION; ++i)
		{
			data.normalMapData[0].EmplaceFast(static_cast<uint8>(CatalystMath::RandomIntegerInRange<uint16>(126, 128)));
			data.normalMapData[0].EmplaceFast(static_cast<uint8>(CatalystMath::RandomIntegerInRange<uint16>(126, 128)));
			data.normalMapData[0].EmplaceFast(255);
			data.normalMapData[0].EmplaceFast(255);
		}

		CreateOceanMaterial(data, defaultOceanMaterial);

		EnvironmentManager::Instance->SetOceanMaterial(defaultOceanMaterial);
	}
}

/*
*	Updates the matrices.
*/
void RenderingSystem::UpdateMatrices() NOEXCEPT
{
	if (activeCamera)
	{
		//Calculate the projection matrix.
		projectionMatrix = Matrix4::Perspective(CatalystMath::DegreesToRadians(activeCamera->GetFieldOfView()), static_cast<float>(GetResolution().width) / static_cast<float>(GetResolution().height), activeCamera->GetNearPlane(), activeCamera->GetFarPlane());
	
		//Calculate the camera matrix.
		cameraMatrix = Matrix4::LookAt(activeCamera->GetPosition(), activeCamera->GetPosition() + activeCamera->GetForwardVector(), activeCamera->GetUpVector());

		//Calculate the view matrix.
		viewMatrix = projectionMatrix * cameraMatrix;
	}
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM