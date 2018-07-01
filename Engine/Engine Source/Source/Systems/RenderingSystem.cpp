//Header file.
#include <Systems/RenderingSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/TerrainEntity.h>

//Math.
#include <Math/CatalystMath.h>

//Rendering.
#include <Rendering/Engine Layer/OceanMaterial.h>
#include <Rendering/Engine Layer/ParticleMaterial.h>
#include <Rendering/Engine Layer/PhysicalMaterial.h>
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/RenderingUtilities.h>
#include <Rendering/Engine Layer/Resolution.h>
#include <Rendering/Engine Layer/VegetationMaterial.h>
#include <Rendering/Engine Layer/Render Passes/RenderPasses.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//Resources.
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/VegetationMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Define the current rendering system.
#if defined(CATALYST_WINDOWS)
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

	//Initialize the common physical models.
	InitializeCommonPhysicalModels();
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
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle RenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the 2D texture via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->Create2DTexture(textureData);
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
	oceanMaterial.normalMapTexture = Create2DTexture(TextureData(TextureDataContainer(waterMaterialData.normalMapData, waterMaterialData.width, waterMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
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
	particleMaterial.albedoTexture = Create2DTexture(TextureData(TextureDataContainer(particleMaterialData.albedoData, particleMaterialData.width, particleMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Create the albedo texture.
	physicalMaterial.albedoTexture = Create2DTexture(TextureData(TextureDataContainer(physicalMaterialData.albedoData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	physicalMaterial.normalMapTexture = Create2DTexture(TextureData(TextureDataContainer(physicalMaterialData.normalMapData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the material properties texture.
	physicalMaterial.materialPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(physicalMaterialData.materialPropertiesData, physicalMaterialData.width, physicalMaterialData.height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the first layer albedo.
	terrainMaterial.firstLayerAlbedo = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.firstLayerAlbedoData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer normal map.
	terrainMaterial.firstLayerNormalMap = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.firstLayerNormalMapData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer material properties.
	terrainMaterial.firstLayerMaterialProperties = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.firstLayerMaterialPropertiesData, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer albedo.
	terrainMaterial.secondLayerAlbedo = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.secondLayerAlbedoData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer normal map.
	terrainMaterial.secondLayerNormalMap = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.secondLayerNormalMapData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer material properties.
	terrainMaterial.secondLayerMaterialProperties = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.secondLayerMaterialPropertiesData, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer albedo.
	terrainMaterial.thirdLayerAlbedo = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerAlbedoData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer normal map.
	terrainMaterial.thirdLayerNormalMap = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerNormalMapData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer material properties.
	terrainMaterial.thirdLayerMaterialProperties = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.thirdLayerMaterialPropertiesData, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer albedo.
	terrainMaterial.fourthLayerAlbedo = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerAlbedoData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer normal map.
	terrainMaterial.fourthLayerNormalMap = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerNormalMapData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer material properties.
	terrainMaterial.fourthLayerMaterialProperties = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fourthLayerMaterialPropertiesData, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer albedo.
	terrainMaterial.fifthLayerAlbedo = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerAlbedoData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer normal map.
	terrainMaterial.fifthLayerNormalMap = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerNormalMapData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer material properties.
	terrainMaterial.fifthLayerMaterialProperties = Create2DTexture(TextureData(TextureDataContainer(terrainMaterialData.fifthLayerMaterialPropertiesData, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a vegetation material.
*/
void RenderingSystem::CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT
{
	//Create the mask texture.
	vegetationMaterial.maskTexture = Create2DTexture(TextureData(TextureDataContainer(vegetationMaterialData.maskData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the albedo texture.
	vegetationMaterial.albedoTexture = Create2DTexture(TextureData(TextureDataContainer(vegetationMaterialData.albedoData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	vegetationMaterial.normalMapTexture = Create2DTexture(TextureData(TextureDataContainer(vegetationMaterialData.normalMapData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the properties texture.
	vegetationMaterial.propertiesTexture = Create2DTexture(TextureData(TextureDataContainer(vegetationMaterialData.propertiesData, vegetationMaterialData.width, vegetationMaterialData.height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
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
	TerrainRenderComponent &component{ ComponentManager::GetTerrainRenderComponents()[entity->GetComponentsIndex()] };

	//Destroy the render data table.
	DestroyRenderDataTable(component.renderDataTable);
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
	renderPasses[INDEX(RenderPassStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::DirectionalStaticPhysicalShadow)] = DirectionalStaticPhysicalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::DirectionalInstancedPhysicalShadow)] = DirectionalInstancedPhysicalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Terrain)] = TerrainRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::StaticPhysical)] = StaticPhysicalRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::InstancedPhysical)] = InstancedPhysicalRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Vegetation)] = VegetationRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::DirectionalShadowHorizontalBlur)] = DirectionalShadowHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::DirectionalShadowVerticalBlur)] = DirectionalShadowVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::SceenSpaceAmbientOcclusion)] = ScreenSpaceAmbientOcclusionRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::SceenSpaceAmbientOcclusionHorizontalBlur)] = ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::SceenSpaceAmbientOcclusionVerticalBlur)] = ScreenSpaceAmbientOcclusionVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Lighting)] = LightingRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Sky)] = SkyRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Ocean)] = OceanRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::Bloom)] = BloomRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::BloomHorizontalBlur)] = BloomHorizontalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::BloomVerticalBlur)] = BloomVerticalBlurRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
	renderPasses[INDEX(RenderPassStage::PostProcessing)] = PostProcessingRenderPass::Instance.Get();
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
*	Initializes the common physical models.
*/
void RenderingSystem::InitializeCommonPhysicalModels() NOEXCEPT
{
	{
		//Create the plane common physical model.
		PhysicalModelData planePhysicalModelData;
		RenderingUtilities::GetPlanePhysicalModelData(planePhysicalModelData);
		CreatePhysicalModel(planePhysicalModelData, commonPhysicalModels[INDEX(CommonPhysicalModel::Plane)]);
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