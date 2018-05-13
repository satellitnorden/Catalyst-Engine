//Header file.
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/InstancedPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/SpotLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Matrix3.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/OceanMaterial.h>
#include <Rendering/Engine Layer/ParticleMaterial.h>
#include <Rendering/Engine Layer/PhysicalMaterial.h>
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/RenderingUtilities.h>
#include <Rendering/Engine Layer/ShaderLoader.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/VegetationMaterial.h>
#include <Rendering/Translation Layer/Vulkan/VulkanParticleSystemProperties.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationUtilities.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/VegetationMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/EnvironmentSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TaskSystem.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanRenderingSystem);

//Vulkan rendering system constants.
namespace VulkanRenderingSystemConstants
{
	constexpr uint32 MAXIMUM_NUMBER_OF_PARTICLES{ 16'384 };
	constexpr VkDeviceSize PARTICLE_SYSTEM_STORAGE_BUFFER_SIZE{ 1'310'736 };
}

/*
*	Initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the main window.
	mainWindow.Initialize();

	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize(mainWindow);

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all semaphores.
	InitializeSemaphores();

	//Initialize all uniform buffers.
	InitializeUniformBuffers();

	//Initialize all descriptor set layouts.
	InitializeDescriptorSetLayouts();

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all pipelines.
	InitializePipelines();

	//Initialize all descriptor sets.
	InitializeDescriptorSets();

	//Initialize all default textures.
	InitializeDefaultTextures();

	//Initialize the Vulkan frame data.
	frameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(), descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)], descriptorSetLayouts[INDEX(DescriptorSetLayout::Environment)], descriptorSetLayouts[INDEX(DescriptorSetLayout::Ocean)]);
}

/*
*	Updates the graphics system synchronously.
*/
void VulkanRenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Execute the asynchronous tasks.
	ExecuteAsynchronousTasks();

	//Update the main window.
	mainWindow.Update();

	//Check if the main window should close - If so, terminate immediately.
	if (mainWindow.ShouldClose())
	{
		EngineSystem::Instance->Terminate();

		return;
	}

	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(semaphores[INDEX(GraphicsSemaphore::ImageAvailable)]);

	//Begin the frame.
	BeginFrame();

	//Update the dynamic uniform data.
	UpdateDynamicUniformData();

	//Update the descriptor sets.
	UpdateDescriptorSets();

	//Execute the frame-dependant asynchronous tasks.
	ExecuteFrameDependantAsynchronousTasks();

	//Concatenate all secondary command buffers into the previous one.
	ConcatenateCommandBuffers();

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[INDEX(GraphicsSemaphore::RenderFinished)]);
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release all descriptor set layouts.
	for (uint32 i = 0; i < INDEX(DescriptorSetLayout::NumberOfDescriptorSetLayouts); ++i)
	{
		descriptorSetLayouts[i].Release();
	}

	//Release the main window.
	mainWindow.Release();

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Creates an environment material.
*/
void VulkanRenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the diffuse texture.
	environmentMaterial.diffuseTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData.diffuseData.Data(), environmentMaterialData.diffuseResolution, environmentMaterialData.diffuseResolution));

	//Create the diffuse irradiance texture.
	environmentMaterial.diffuseIrradianceTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData.diffuseIrradianceData.Data(), environmentMaterialData.diffuseIrradianceResolution, environmentMaterialData.diffuseIrradianceResolution));
}

/*
*	Creates a terrain material.
*/
void VulkanRenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Load the first layer albedo.
	terrainMaterial.firstLayerAlbedo = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4, terrainMaterialData.firstLayerAlbedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the first layer normal map.
	terrainMaterial.firstLayerNormalMap = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4, terrainMaterialData.firstLayerNormalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the first layer material properties.
	terrainMaterial.firstLayerMaterialProperties = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, 4, terrainMaterialData.firstLayerMaterialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the second layer albedo.
	terrainMaterial.secondLayerAlbedo = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4, terrainMaterialData.secondLayerAlbedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the second layer normal map.
	terrainMaterial.secondLayerNormalMap = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4, terrainMaterialData.secondLayerNormalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the second layer material properties.
	terrainMaterial.secondLayerMaterialProperties = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, 4, terrainMaterialData.secondLayerMaterialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the third layer albedo.
	terrainMaterial.thirdLayerAlbedo = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4, terrainMaterialData.thirdLayerAlbedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the third layer normal map.
	terrainMaterial.thirdLayerNormalMap = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4, terrainMaterialData.thirdLayerNormalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the third layer material properties.
	terrainMaterial.thirdLayerMaterialProperties = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, 4, terrainMaterialData.thirdLayerMaterialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fourth layer albedo.
	terrainMaterial.fourthLayerAlbedo = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4, terrainMaterialData.fourthLayerAlbedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fourth layer normal map.
	terrainMaterial.fourthLayerNormalMap = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4, terrainMaterialData.fourthLayerNormalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fourth layer material properties.
	terrainMaterial.fourthLayerMaterialProperties = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, 4, terrainMaterialData.fourthLayerMaterialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fifth layer albedo.
	terrainMaterial.fifthLayerAlbedo = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4, terrainMaterialData.fifthLayerAlbedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fifth layer normal map.
	terrainMaterial.fifthLayerNormalMap = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4, terrainMaterialData.fifthLayerNormalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the fifth layer material properties.
	terrainMaterial.fifthLayerMaterialProperties = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, 4, terrainMaterialData.fifthLayerMaterialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));
}

/*
*	Creates a particle material.
*/
void VulkanRenderingSystem::CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT
{
	//Load the albedo.
	particleMaterial.albedoTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(particleMaterialData.width, particleMaterialData.height, 4, particleMaterialData.albedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));
}

/*
*	Creates a physical material.
*/
void VulkanRenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Load the albedo.
	physicalMaterial.albedoTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(physicalMaterialData.width, physicalMaterialData.height, 4, physicalMaterialData.albedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the normal map.
	physicalMaterial.normalMapTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(physicalMaterialData.width, physicalMaterialData.height, 4, physicalMaterialData.normalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));

	//Load the material properties.
	physicalMaterial.materialPropertiesTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(physicalMaterialData.width, physicalMaterialData.height, 4, physicalMaterialData.materialPropertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));
}

/*
*	Creates a vegetation material.
*/
void VulkanRenderingSystem::CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT
{
	//Load the mask texture.
	vegetationMaterial.maskTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(vegetationMaterialData.width, vegetationMaterialData.height, 4, vegetationMaterialData.maskData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE));

	//Load the albedo texture.
	vegetationMaterial.albedoTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(vegetationMaterialData.width, vegetationMaterialData.height, 4, vegetationMaterialData.albedoData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE));

	//Load the normal map texture.
	vegetationMaterial.normalMapTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(vegetationMaterialData.width, vegetationMaterialData.height, 4, vegetationMaterialData.normalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE));

	//Load the properties texture.
	vegetationMaterial.propertiesTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(vegetationMaterialData.width, vegetationMaterialData.height, 4, vegetationMaterialData.propertiesData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE));
}

/*
*	Creates and returns physical model.
*/
void VulkanRenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ physicalModelData.vertices.Data(), physicalModelData.indices.Data() };
	const VkDeviceSize modelDataSizes[]{ sizeof(PhysicalVertex) * physicalModelData.vertices.Size(), sizeof(uint32) * physicalModelData.indices.Size() };
	GraphicsBufferHandle buffer = VulkanInterface::Instance->CreateConstantBuffer(modelData, modelDataSizes, 2);

	//Set up the physical model.
	physicalModel.GetAxisAlignedBoundingBox().minimum = Vector3(-physicalModelData.extent, -physicalModelData.extent, -physicalModelData.extent);
	physicalModel.GetAxisAlignedBoundingBox().maximum = Vector3(physicalModelData.extent, physicalModelData.extent, physicalModelData.extent);
	physicalModel.SetBuffer(buffer);
	physicalModel.SetIndexOffset(modelDataSizes[0]);
	physicalModel.SetIndexCount(static_cast<uint32>(physicalModelData.indices.Size()));
}

/*
*	Creates a water material.
*/
void VulkanRenderingSystem::CreateWaterMaterial(const WaterMaterialData &waterMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT
{
	//Load the normal map.
	oceanMaterial.normalMapTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(waterMaterialData.width, waterMaterialData.height, 4, waterMaterialData.normalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));
}

/*
*	Initializes a terrain entity.
*/
void VulkanRenderingSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture2D &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT
{
	//Generate the terrain plane vertices and indices.
	DynamicArray<float> terrainVertices;
	DynamicArray<uint32> terrainIndices;
	RenderingUtilities::GenerateTerrainPlane(terrainProperties, terrainUniformData, terrainPlaneResolution, terrainVertices, terrainIndices);

	//Create the vertex and index buffer.
	const void *RESTRICT terrainData[]{ terrainVertices.Data(), terrainIndices.Data() };
	const VkDeviceSize terrainDataSizes[]{ sizeof(float) * terrainVertices.Size(), sizeof(uint32) * terrainIndices.Size() };
	VulkanConstantBuffer *RESTRICT terrainVertexBuffer{ VulkanInterface::Instance->CreateConstantBuffer(terrainData, terrainDataSizes, 2) };

	//Fill the terrain entity components with the relevant data.
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainComponents()[terrainEntity.GetComponentsIndex()] };
	TerrainRenderComponent &terrainRenderComponent{ ComponentManager::GetTerrainRenderComponents()[terrainEntity.GetComponentsIndex()] };

	terrainComponent.terrainUniformData = terrainUniformData;
	terrainComponent.uniformBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(TerrainUniformData));
	static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent.uniformBuffer)->UploadData(&terrainComponent.terrainUniformData);
	terrainComponent.terrainProperties = terrainProperties;

	//Create the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(terrainRenderComponent.descriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Terrain)]);

	DynamicArray<VkWriteDescriptorSet, 18> writeDescriptorSets;

	Texture2DHandle terrainPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(terrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	writeDescriptorSets.EmplaceFast(static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent.uniformBuffer)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 1));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainPropertiesTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 2));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(layerWeightsTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 3));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerAlbedo)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 4));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerNormalMap)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 5));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerMaterialProperties)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 6));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerAlbedo)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 7));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerNormalMap)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 8));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerMaterialProperties)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 9));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerAlbedo)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 10));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerNormalMap)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 11));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerMaterialProperties)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 12));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerAlbedo)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 13));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerNormalMap)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 14));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerMaterialProperties)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 15));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerAlbedo)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 16));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerNormalMap)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 17));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerMaterialProperties)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 18));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	terrainRenderComponent.vertexAndIndexBuffer = terrainVertexBuffer;
	terrainRenderComponent.indexBufferOffset = static_cast<uint32>(sizeof(float) * terrainVertices.Size());
	terrainRenderComponent.indexCount = static_cast<uint32>(terrainIndices.Size());
}

/*
*	Initializes a static physical entity.
*/
void VulkanRenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	const PhysicalMaterial &material = model.GetMaterial();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
	{
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.materialPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Fill the static physical entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	StaticPhysicalRenderComponent &renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	TransformComponent &transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents()[staticPhysicalEntity.GetComponentsIndex()] };

	frustumCullingComponent.axisAlignedBoundingBox = model.GetAxisAlignedBoundingBox();
	renderComponent.modelMatrix = Matrix4(position, rotation, scale);
	renderComponent.descriptorSet = newDescriptorSet;
	renderComponent.buffer = *static_cast<VulkanConstantBuffer *RESTRICT>(model.GetBuffer());
	renderComponent.indexOffset = model.GetIndexOffset();
	renderComponent.indexCount = model.GetIndexCount();
	transformComponent.position = position;
	transformComponent.rotation = rotation;
	transformComponent.scale = scale;
}

/*
*	Initializes an instanced physical entity.
*/
void VulkanRenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	const PhysicalMaterial &material = model.GetMaterial();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
	{
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.materialPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ transformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(Matrix4) * transformations.Size() };
	VulkanConstantBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the instanced physical entity components with the relevant data.
	InstancedPhysicalRenderComponent &renderComponent{ ComponentManager::GetInstancedPhysicalRenderComponents()[entity.GetComponentsIndex()] };

	renderComponent.descriptorSet = newDescriptorSet;
	renderComponent.modelBuffer = model.GetBuffer();
	renderComponent.transformationsBuffer = transformationsBuffer;
	renderComponent.indexOffset = model.GetIndexOffset();
	renderComponent.indexCount = model.GetIndexCount();
	renderComponent.instanceCount = static_cast<uint32>(transformations.Size());
}

/*
*	Initializes a vegetation entity.
*/
void VulkanRenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Get the components.
	VegetationComponent &renderComponent{ ComponentManager::GetVegetationComponents()[entity.GetComponentsIndex()] };
	VegetationCullingComponent &cullingComponent{ ComponentManager::GetVegetationCullingComponents()[entity.GetComponentsIndex()] };

	//Calculate the vegetation grid.
	DynamicArray<VegetationTransformation> sortedTransformations;
	RenderingUtilities::CalculateVegetationGrid(properties.cutoffDistance, transformations, &renderComponent, &cullingComponent, sortedTransformations);

	//The cutoff distance is used as a squared value in the shadres, so square it.
	VegetationProperties shaderProperties{ properties };
	shaderProperties.cutoffDistance *= shaderProperties.cutoffDistance;

	//Create the vegetation properties uniform buffer.
	VulkanUniformBuffer *const RESTRICT propertiesBuffer{ VulkanInterface::Instance->CreateUniformBuffer(sizeof(VegetationProperties)) };
	propertiesBuffer->UploadData(&shaderProperties);

	//Create the descriptor set.
	VulkanDescriptorSet newDescriptorSet;

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Vegetation)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 5> writeDescriptorSets
	{
		propertiesBuffer->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.maskTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 3),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 4),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.propertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 5)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ sortedTransformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(VegetationTransformation) * sortedTransformations.Size() };
	VulkanConstantBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the components with the relevant data.
	renderComponent.descriptorSet = newDescriptorSet;
	renderComponent.transformationsBuffer = transformationsBuffer;
}

/*
*	Initializes a particle system entity.
*/
void VulkanRenderingSystem::InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT
{
	//Create the storage buffer.
	VulkanStorageBuffer *const RESTRICT storageBuffer{ VulkanInterface::Instance->CreateStorageBuffer(VulkanRenderingSystemConstants::PARTICLE_SYSTEM_STORAGE_BUFFER_SIZE) };

	//Create the uniform buffer.
	VulkanUniformBuffer *const RESTRICT uniformBuffer{ VulkanInterface::Instance->CreateUniformBuffer(static_cast<VkDeviceSize>(sizeof(VulkanParticleSystemProperties))) };
	uniformBuffer->UploadData(&VulkanParticleSystemProperties(properties));

	//Create the descriptor set.
	VulkanDescriptorSet descriptorSet;
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::ParticleSystem)]);

	StaticArray<VkWriteDescriptorSet, 3> particleSystemWriteDescriptorSets
	{
		storageBuffer->GetWriteDescriptorSet(descriptorSet, 1),
		uniformBuffer->GetWriteDescriptorSet(descriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(descriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(particleSystemWriteDescriptorSets.Size()), particleSystemWriteDescriptorSets.Data(), 0, nullptr);

	//Set up the particle system's components.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemComponents()[entity.GetComponentsIndex()] };
	ParticleSystemRenderComponent &renderComponent{ ComponentManager::GetParticleSystemRenderComponents()[entity.GetComponentsIndex()] };

	component.properties = properties;
	component.propertiesUniformBuffer = uniformBuffer;
	renderComponent.descriptorSet = descriptorSet;
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle VulkanRenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the Vulkan 2D texture.
	Vulkan2DTexture *RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), textureData.textureDataContainer.textureWidth, textureData.textureDataContainer.textureHeight, textureData.textureDataContainer.textureChannels, textureData.textureDataContainer.textureTexelSize, textureData.textureDataContainer.textureData.Data(), VulkanTranslationUtilities::GetVulkanFormat(textureData.textureFormat), VulkanTranslationUtilities::GetVulkanTextureFilter(textureData.magnificationFilter), VulkanTranslationUtilities::GetVulkanMipmapMode(textureData.mipmapMode), VulkanTranslationUtilities::GetVulkanAddressMode(textureData.addressMode));

	//Return the texture.
	return static_cast<Texture2DHandle>(new2DTexture);
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void VulkanRenderingSystem::SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera.
	activeCamera = newActiveCamera;

	//Calculate the projection matrix.
	CalculateProjectionMatrix();
}

/*
*	Sets the post processing blur amount.
*/
void VulkanRenderingSystem::SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.blurAmount = newBlurAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing chromatic aberration amount.
*/
void VulkanRenderingSystem::SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.chromaticAberrationAmount = newChromaticAberrationAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing saturation.
*/
void VulkanRenderingSystem::SetPostProcessingSaturation(const float newSaturation) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.saturation = newSaturation;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing sharpen amount.
*/
void VulkanRenderingSystem::SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.sharpenAmount = newSharpenAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all render targets.
*/
void VulkanRenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all depth buffers.
	depthBuffers[INDEX(DepthBuffer::DirectionalLight)] = VulkanInterface::Instance->CreateDepthBuffer(VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION });
	depthBuffers[INDEX(DepthBuffer::SceneBuffer)] = VulkanInterface::Instance->CreateDepthBuffer(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Initialize all render targets.
	renderTargets[INDEX(RenderTarget::DirectionalPreBlurShadowMap)] = VulkanInterface::Instance->CreateRenderTarget(VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::DirectionalPostBlurShadowMap)] = VulkanInterface::Instance->CreateRenderTarget(VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::Scene)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::WaterScene)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

/*
*	Initializes all semaphores.
*/
void VulkanRenderingSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	semaphores[INDEX(GraphicsSemaphore::ImageAvailable)] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[INDEX(GraphicsSemaphore::RenderFinished)] = VulkanInterface::Instance->CreateSemaphore();
}

/*
*	Initializes all uniform buffers.
*/
void VulkanRenderingSystem::InitializeUniformBuffers() NOEXCEPT
{
	//Create the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(PostProcessingUniformData));

	//Upload the initial data to the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all descriptor set layouts.
*/
void VulkanRenderingSystem::InitializeDescriptorSetLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> dynamicUniformDataDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)].Initialize(1, dynamicUniformDataDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the shadow map blur descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> shadowMapBlurDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::ShadowMapBlur)].Initialize(static_cast<uint32>(shadowMapBlurDescriptorSetLayoutBindings.Size()), shadowMapBlurDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the environment descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> environmentDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Environment)].Initialize(static_cast<uint32>(environmentDescriptorSetLayoutBindings.Size()), environmentDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the terrain descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 18> terrainDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(11, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(12, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(13, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(14, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(15, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(16, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(17, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(18, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Terrain)].Initialize(18, terrainDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the static physical descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> staticPhysicalDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)].Initialize(3, staticPhysicalDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the vegetation descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 5> vegetationDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Vegetation)].Initialize(static_cast<uint32>(vegetationDescriptorSetLayoutBindings.Size()), vegetationDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the lighting descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> lightingDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)].Initialize(static_cast<uint32>(lightingDescriptorSetLayoutBindings.Size()), lightingDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the ocean descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> oceanDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::Ocean)].Initialize(static_cast<uint32>(oceanDescriptorSetLayoutBindings.Size()), oceanDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the particle system descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> particleSystemDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::ParticleSystem)].Initialize(static_cast<uint32>(particleSystemDescriptorSetLayoutBindings.Size()), particleSystemDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the post processing descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> postProcessingDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)].Initialize(2, postProcessingDescriptorSetLayoutBindings.Data());
	}
}

/*
*	Initializes all shader modules.
*/
void VulkanRenderingSystem::InitializeShaderModules() NOEXCEPT
{
	//Initialize the cube map fragment shader module.
	const DynamicString &shadersPath{ EngineSystem::Instance->GetProjectInformation().renderingInformation.shadersPath };

	{
		//Initialize the cube map fragment shader module.
		const DynamicArray<char> cubeMapFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "CubeMapFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::CubeMapFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(cubeMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the cube map vertex shader module.
		const DynamicArray<char> cubeMapVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "CubeMapVertexShader.spv");
		shaderModules[INDEX(ShaderModule::CubeMapVertexShader)] = VulkanInterface::Instance->CreateShaderModule(cubeMapVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadowInstanced physical vertex shader module.
		const DynamicArray<char> directionalShadowInstancedPhysicalVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "DirectionalShadowInstancedPhysicalVertexShader.spv");
		shaderModules[INDEX(ShaderModule::DirectionalShadowInstancedPhysicalVertexShader)] = VulkanInterface::Instance->CreateShaderModule(directionalShadowInstancedPhysicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		const DynamicArray<char> directionalShadowTerrainTessellationEvaluationShaderByteCode = ShaderLoader::LoadShader(shadersPath + "DirectionalShadowTerrainTessellationEvaluationShader.spv");
		shaderModules[INDEX(ShaderModule::DirectionalShadowTerrainTessellationEvaluationShader)] = VulkanInterface::Instance->CreateShaderModule(directionalShadowTerrainTessellationEvaluationShaderByteCode, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize the instanced physical vertex shader module.
		const DynamicArray<char> instancedPhysicalVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "InstancedPhysicalVertexShader.spv");
		shaderModules[INDEX(ShaderModule::InstancedPhysicalVertexShader)] = VulkanInterface::Instance->CreateShaderModule(instancedPhysicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the lighting fragment shader module.
		const DynamicArray<char> lightingFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "LightingFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::LightingFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(lightingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the ocean fragment shader module.
		const DynamicArray<char> oceanFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "OceanFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::OceanFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(oceanFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the particle system fragment shader module.
		const DynamicArray<char> particleSystemFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ParticleSystemFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::ParticleSystemFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(particleSystemFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the particle system geometry shader module.
		const DynamicArray<char> particleSystemGeometryShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ParticleSystemGeometryShader.spv");
		shaderModules[INDEX(ShaderModule::ParticleSystemGeometryShader)] = VulkanInterface::Instance->CreateShaderModule(particleSystemGeometryShaderByteCode, VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the particle system vertex shader module.
		const DynamicArray<char> particleSystemVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ParticleSystemVertexShader.spv");
		shaderModules[INDEX(ShaderModule::ParticleSystemVertexShader)] = VulkanInterface::Instance->CreateShaderModule(particleSystemVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the post processing fragment shader module.
		const DynamicArray<char> postProcessingFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PostProcessingFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::PostProcessingFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(postProcessingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical fragment shader module.
		const DynamicArray<char> physicalFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PhysicalFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::PhysicalFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(physicalFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical vertex shader module.
		const DynamicArray<char> physicalVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PhysicalVertexShader.spv");
		shaderModules[INDEX(ShaderModule::PhysicalVertexShader)] = VulkanInterface::Instance->CreateShaderModule(physicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the shadow map blur fragment shader module.
		const DynamicArray<char> shadowMapBlurFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ShadowMapBlurFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::ShadowMapBlurFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(shadowMapBlurFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the shadow map fragment shader module.
		const DynamicArray<char> shadowMapFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ShadowMapFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::ShadowMapFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(shadowMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain scene buffer fragment shader module.
		const DynamicArray<char> terrainSceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::TerrainFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation control shader module.
		const DynamicArray<char> terrainSceneBufferTessellationControlShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferTessellationControlShader.spv");
		shaderModules[INDEX(ShaderModule::TerrainTessellationControlShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationControlShaderByteCode, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation evaluation shader module.
		const DynamicArray<char> terrainSceneBufferTessellationEvaluationShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferTessellationEvaluationShader.spv");
		shaderModules[INDEX(ShaderModule::TerrainTessellationEvaluationShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationEvaluationShaderByteCode, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}
	
	{
		//Initialize the terrain scene buffer vertex shader module.
		const DynamicArray<char> terrainSceneBufferVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferVertexShader.spv");
		shaderModules[INDEX(ShaderModule::TerrainVertexShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the vegetation fragment shader module.
		const DynamicArray<char> vegetationFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationFragmentShader.spv");
		shaderModules[INDEX(ShaderModule::VegetationFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the vegetation geometry shader module.
		const DynamicArray<char> vegetationGeometryShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationGeometryShader.spv");
		shaderModules[INDEX(ShaderModule::VegetationGeometryShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationGeometryShaderByteCode, VK_SHADER_STAGE_GEOMETRY_BIT);
	}
	
	{
		//Initialize the vegetation vertex shader module.
		const DynamicArray<char> vegetationVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationVertexShader.spv");
		shaderModules[INDEX(ShaderModule::VegetationVertexShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the viewport vertex shader module.
		const DynamicArray<char> viewportVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ViewportVertexShader.spv");
		shaderModules[INDEX(ShaderModule::ViewportVertexShader)] = VulkanInterface::Instance->CreateShaderModule(viewportVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
	}
}

/*
*	Initializes all pipelines.
*/
void VulkanRenderingSystem::InitializePipelines() NOEXCEPT
{
	{
		//Create the terrain shadow pipeline.
		VulkanPipelineCreationParameters directionalShadowTerrainPipelineCreationParameters;

		directionalShadowTerrainPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
		directionalShadowTerrainPipelineCreationParameters.blendEnable = false;
		directionalShadowTerrainPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		directionalShadowTerrainPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		directionalShadowTerrainPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		directionalShadowTerrainPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		directionalShadowTerrainPipelineCreationParameters.colorAttachments[0].Reserve(1);
		directionalShadowTerrainPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::DirectionalPreBlurShadowMap)]->GetImageView());
		directionalShadowTerrainPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		directionalShadowTerrainPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		directionalShadowTerrainPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		directionalShadowTerrainPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		directionalShadowTerrainPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::DirectionalLight)];
		directionalShadowTerrainPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		directionalShadowTerrainPipelineCreationParameters.depthTestEnable = VK_TRUE;
		directionalShadowTerrainPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> terrainShadowDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Terrain)]
		};
		directionalShadowTerrainPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(terrainShadowDescriptorSetLayouts.Size());
		directionalShadowTerrainPipelineCreationParameters.descriptorSetLayouts = terrainShadowDescriptorSetLayouts.Data();
		directionalShadowTerrainPipelineCreationParameters.pushConstantRangeCount = 0;
		directionalShadowTerrainPipelineCreationParameters.pushConstantRanges = nullptr;
		directionalShadowTerrainPipelineCreationParameters.shaderModules.Reserve(4);
		directionalShadowTerrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainVertexShader)]);
		directionalShadowTerrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainTessellationControlShader)]);
		directionalShadowTerrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::DirectionalShadowTerrainTessellationEvaluationShader)]);
		directionalShadowTerrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ShadowMapFragmentShader)]);
		directionalShadowTerrainPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		StaticArray<VkVertexInputAttributeDescription, 2> terrainShadowVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetTerrainVertexInputAttributeDescriptions(terrainShadowVertexInputAttributeDescriptions);
		directionalShadowTerrainPipelineCreationParameters.vertexInputAttributeDescriptionCount = static_cast<uint32>(terrainShadowVertexInputAttributeDescriptions.Size());
		directionalShadowTerrainPipelineCreationParameters.vertexInputAttributeDescriptions = terrainShadowVertexInputAttributeDescriptions.Data();
		VkVertexInputBindingDescription terrainShadowVertexInputBindingDescription;
		VulkanTranslationUtilities::GetTerrainVertexInputBindingDescription(terrainShadowVertexInputBindingDescription);
		directionalShadowTerrainPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
		directionalShadowTerrainPipelineCreationParameters.vertexInputBindingDescriptions = &terrainShadowVertexInputBindingDescription;
		directionalShadowTerrainPipelineCreationParameters.viewportExtent = VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION };

		pipelines[INDEX(Pipeline::DirectionalShadowTerrain)] = VulkanInterface::Instance->CreatePipeline(directionalShadowTerrainPipelineCreationParameters);
	}

	{
		//Create the directional shadow instanced physical pipeline.
		VulkanPipelineCreationParameters directionalShadowInstancedPhysicalPipelineCreationParameters;

		directionalShadowInstancedPhysicalPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		directionalShadowInstancedPhysicalPipelineCreationParameters.blendEnable = false;
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachments[0].Reserve(1);
		directionalShadowInstancedPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::DirectionalPreBlurShadowMap)]->GetImageView());
		directionalShadowInstancedPhysicalPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::DirectionalLight)];
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthTestEnable = VK_TRUE;
		directionalShadowInstancedPhysicalPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> directionalShadowInstancedDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)]
		};
		directionalShadowInstancedPhysicalPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(directionalShadowInstancedDescriptorSetLayouts.Size());
		directionalShadowInstancedPhysicalPipelineCreationParameters.descriptorSetLayouts = directionalShadowInstancedDescriptorSetLayouts.Data();
		directionalShadowInstancedPhysicalPipelineCreationParameters.pushConstantRangeCount = 0;
		directionalShadowInstancedPhysicalPipelineCreationParameters.pushConstantRanges = nullptr;
		directionalShadowInstancedPhysicalPipelineCreationParameters.shaderModules.Reserve(2);
		directionalShadowInstancedPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::DirectionalShadowInstancedPhysicalVertexShader)]);
		directionalShadowInstancedPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ShadowMapFragmentShader)]);
		directionalShadowInstancedPhysicalPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		StaticArray<VkVertexInputAttributeDescription, 8> directionalShadowInstancedPhysicalVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetInstancedPhysicalVertexInputAttributeDescriptions(directionalShadowInstancedPhysicalVertexInputAttributeDescriptions);
		directionalShadowInstancedPhysicalPipelineCreationParameters.vertexInputAttributeDescriptionCount = static_cast<uint32>(directionalShadowInstancedPhysicalVertexInputAttributeDescriptions.Size());
		directionalShadowInstancedPhysicalPipelineCreationParameters.vertexInputAttributeDescriptions = directionalShadowInstancedPhysicalVertexInputAttributeDescriptions.Data();
		StaticArray<VkVertexInputBindingDescription, 2> directionalShadowInstancedPhysicalVertexInputBindingDescriptions;
		VulkanTranslationUtilities::GetInstancedPhysicalVertexInputBindingDescriptions(directionalShadowInstancedPhysicalVertexInputBindingDescriptions);
		directionalShadowInstancedPhysicalPipelineCreationParameters.vertexInputBindingDescriptionCount = static_cast<uint32>(directionalShadowInstancedPhysicalVertexInputBindingDescriptions.Size());
		directionalShadowInstancedPhysicalPipelineCreationParameters.vertexInputBindingDescriptions = directionalShadowInstancedPhysicalVertexInputBindingDescriptions.Data();
		directionalShadowInstancedPhysicalPipelineCreationParameters.viewportExtent = VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION };

		pipelines[INDEX(Pipeline::DirectionalShadowInstancedPhysical)] = VulkanInterface::Instance->CreatePipeline(directionalShadowInstancedPhysicalPipelineCreationParameters);
	}

	{
		//Create the shadow map blur pipeline.
		VulkanPipelineCreationParameters shadowMapBlurPipelineCreationParameters;

		shadowMapBlurPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
		shadowMapBlurPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		shadowMapBlurPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		shadowMapBlurPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		shadowMapBlurPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		shadowMapBlurPipelineCreationParameters.colorAttachments[0].Reserve(1);
		shadowMapBlurPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::DirectionalPostBlurShadowMap)]->GetImageView());
		shadowMapBlurPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		shadowMapBlurPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		shadowMapBlurPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		shadowMapBlurPipelineCreationParameters.depthBuffer = nullptr;
		shadowMapBlurPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		shadowMapBlurPipelineCreationParameters.depthTestEnable = VK_FALSE;
		shadowMapBlurPipelineCreationParameters.depthWriteEnable = VK_FALSE;
		StaticArray<VulkanDescriptorSetLayout, 2> shadowMapBlurDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::ShadowMapBlur)]
		};
		shadowMapBlurPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(shadowMapBlurDescriptorSetLayouts.Size());
		shadowMapBlurPipelineCreationParameters.descriptorSetLayouts = shadowMapBlurDescriptorSetLayouts.Data();
		shadowMapBlurPipelineCreationParameters.pushConstantRangeCount = 0;
		shadowMapBlurPipelineCreationParameters.pushConstantRanges = nullptr;
		shadowMapBlurPipelineCreationParameters.shaderModules.Reserve(2);
		shadowMapBlurPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
		shadowMapBlurPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ShadowMapBlurFragmentShader)]);
		shadowMapBlurPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		shadowMapBlurPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		shadowMapBlurPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		shadowMapBlurPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		shadowMapBlurPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		shadowMapBlurPipelineCreationParameters.viewportExtent = VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION };

		pipelines[INDEX(Pipeline::ShadowMapBlur)] = VulkanInterface::Instance->CreatePipeline(shadowMapBlurPipelineCreationParameters);
	}

	{
		//Create the terrain pipeline.
		VulkanPipelineCreationParameters terrainPipelineCreationParameters;

		terrainPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
		terrainPipelineCreationParameters.blendEnable = false;
		terrainPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		terrainPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		terrainPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		terrainPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		terrainPipelineCreationParameters.colorAttachments[0].Reserve(3);
		terrainPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetImageView());
		terrainPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetImageView());
		terrainPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetImageView());
		terrainPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		terrainPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		terrainPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		terrainPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		terrainPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		terrainPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		terrainPipelineCreationParameters.depthTestEnable = VK_TRUE;
		terrainPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> terrainDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Terrain)]
		};
		terrainPipelineCreationParameters.descriptorSetLayoutCount = 2;
		terrainPipelineCreationParameters.descriptorSetLayouts = terrainDescriptorSetLayouts.Data();
		terrainPipelineCreationParameters.pushConstantRangeCount = 0;
		terrainPipelineCreationParameters.pushConstantRanges = nullptr;
		terrainPipelineCreationParameters.shaderModules.Reserve(4);
		terrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainVertexShader)]);
		terrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainTessellationControlShader)]);
		terrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainTessellationEvaluationShader)]);
		terrainPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::TerrainFragmentShader)]);
		terrainPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		StaticArray<VkVertexInputAttributeDescription, 2> terrainVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetTerrainVertexInputAttributeDescriptions(terrainVertexInputAttributeDescriptions);
		terrainPipelineCreationParameters.vertexInputAttributeDescriptionCount = 2;
		terrainPipelineCreationParameters.vertexInputAttributeDescriptions = terrainVertexInputAttributeDescriptions.Data();
		VkVertexInputBindingDescription terrainVertexInputBindingDescription;
		VulkanTranslationUtilities::GetTerrainVertexInputBindingDescription(terrainVertexInputBindingDescription);
		terrainPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
		terrainPipelineCreationParameters.vertexInputBindingDescriptions = &terrainVertexInputBindingDescription;
		terrainPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::Terrain)] = VulkanInterface::Instance->CreatePipeline(terrainPipelineCreationParameters);
	}

	{
		//Create the static physical pipeline.
		VulkanPipelineCreationParameters staticPhysicalPipelineCreationParameters;

		staticPhysicalPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		staticPhysicalPipelineCreationParameters.blendEnable = false;
		staticPhysicalPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		staticPhysicalPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		staticPhysicalPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		staticPhysicalPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		staticPhysicalPipelineCreationParameters.colorAttachments[0].Reserve(3);
		staticPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetImageView());
		staticPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetImageView());
		staticPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetImageView());
		staticPhysicalPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		staticPhysicalPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		staticPhysicalPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		staticPhysicalPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		staticPhysicalPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		staticPhysicalPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		staticPhysicalPipelineCreationParameters.depthTestEnable = VK_TRUE;
		staticPhysicalPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> sceneBufferDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)]
		};
		staticPhysicalPipelineCreationParameters.descriptorSetLayoutCount = 2;
		staticPhysicalPipelineCreationParameters.descriptorSetLayouts = sceneBufferDescriptorSetLayouts.Data();
		VkPushConstantRange staticPhysicalPushConstantRange{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4) };
		staticPhysicalPipelineCreationParameters.pushConstantRangeCount = 1;
		staticPhysicalPipelineCreationParameters.pushConstantRanges = &staticPhysicalPushConstantRange;
		staticPhysicalPipelineCreationParameters.shaderModules.Reserve(2);
		staticPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::PhysicalVertexShader)]);
		staticPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::PhysicalFragmentShader)]);
		staticPhysicalPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		StaticArray<VkVertexInputAttributeDescription, 4> staticPhysicalVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetPhysicalVertexInputAttributeDescriptions(staticPhysicalVertexInputAttributeDescriptions);
		staticPhysicalPipelineCreationParameters.vertexInputAttributeDescriptionCount = 4;
		staticPhysicalPipelineCreationParameters.vertexInputAttributeDescriptions = staticPhysicalVertexInputAttributeDescriptions.Data();
		VkVertexInputBindingDescription staticPhysicalVertexInputBindingDescription;
		VulkanTranslationUtilities::GetPhysicalVertexInputBindingDescription(staticPhysicalVertexInputBindingDescription);
		staticPhysicalPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
		staticPhysicalPipelineCreationParameters.vertexInputBindingDescriptions = &staticPhysicalVertexInputBindingDescription;
		staticPhysicalPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::StaticPhysical)] = VulkanInterface::Instance->CreatePipeline(staticPhysicalPipelineCreationParameters);
	}

	{
		//Create the instanced physical pipeline.
		VulkanPipelineCreationParameters instancedPhysicalPipelineCreationParameters;

		instancedPhysicalPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		instancedPhysicalPipelineCreationParameters.blendEnable = false;
		instancedPhysicalPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		instancedPhysicalPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		instancedPhysicalPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		instancedPhysicalPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		instancedPhysicalPipelineCreationParameters.colorAttachments[0].Reserve(3);
		instancedPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetImageView());
		instancedPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetImageView());
		instancedPhysicalPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetImageView());
		instancedPhysicalPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		instancedPhysicalPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		instancedPhysicalPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		instancedPhysicalPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		instancedPhysicalPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		instancedPhysicalPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		instancedPhysicalPipelineCreationParameters.depthTestEnable = VK_TRUE;
		instancedPhysicalPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> instancedPhysicalDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)]
		};
		instancedPhysicalPipelineCreationParameters.descriptorSetLayoutCount = 2;
		instancedPhysicalPipelineCreationParameters.descriptorSetLayouts = instancedPhysicalDescriptorSetLayouts.Data();
		instancedPhysicalPipelineCreationParameters.pushConstantRangeCount = 0;
		instancedPhysicalPipelineCreationParameters.pushConstantRanges = nullptr;
		instancedPhysicalPipelineCreationParameters.shaderModules.Reserve(2);
		instancedPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::InstancedPhysicalVertexShader)]);
		instancedPhysicalPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::PhysicalFragmentShader)]);
		instancedPhysicalPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		StaticArray<VkVertexInputAttributeDescription, 8> instancedPhysicalVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetInstancedPhysicalVertexInputAttributeDescriptions(instancedPhysicalVertexInputAttributeDescriptions);
		instancedPhysicalPipelineCreationParameters.vertexInputAttributeDescriptionCount = 8;
		instancedPhysicalPipelineCreationParameters.vertexInputAttributeDescriptions = instancedPhysicalVertexInputAttributeDescriptions.Data();
		StaticArray<VkVertexInputBindingDescription, 2> instancedPhysicalVertexInputBindingDescriptions;
		VulkanTranslationUtilities::GetInstancedPhysicalVertexInputBindingDescriptions(instancedPhysicalVertexInputBindingDescriptions);
		instancedPhysicalPipelineCreationParameters.vertexInputBindingDescriptionCount = 2;
		instancedPhysicalPipelineCreationParameters.vertexInputBindingDescriptions = instancedPhysicalVertexInputBindingDescriptions.Data();
		instancedPhysicalPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::InstancedPhysical)] = VulkanInterface::Instance->CreatePipeline(instancedPhysicalPipelineCreationParameters);
	}

	{
		//Create the vegetation pipeline.
		VulkanPipelineCreationParameters vegetationPipelineCreationParameters;

		vegetationPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		vegetationPipelineCreationParameters.blendEnable = false;
		vegetationPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		vegetationPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		vegetationPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		vegetationPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		vegetationPipelineCreationParameters.colorAttachments[0].Reserve(3);
		vegetationPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetImageView());
		vegetationPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetImageView());
		vegetationPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetImageView());
		vegetationPipelineCreationParameters.cullMode = VK_CULL_MODE_NONE;
		vegetationPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		vegetationPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		vegetationPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		vegetationPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		vegetationPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		vegetationPipelineCreationParameters.depthTestEnable = VK_TRUE;
		vegetationPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> vegetationDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Vegetation)]
		};
		vegetationPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(vegetationDescriptorSetLayouts.Size());
		vegetationPipelineCreationParameters.descriptorSetLayouts = vegetationDescriptorSetLayouts.Data();
		vegetationPipelineCreationParameters.pushConstantRangeCount = 0;
		vegetationPipelineCreationParameters.pushConstantRanges = nullptr;
		vegetationPipelineCreationParameters.shaderModules.Reserve(3);
		vegetationPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::VegetationVertexShader)]);
		vegetationPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::VegetationGeometryShader)]);
		vegetationPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::VegetationFragmentShader)]);
		vegetationPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		StaticArray<VkVertexInputAttributeDescription, 3> vegetationVertexInputAttributeDescriptions;
		VulkanTranslationUtilities::GetVegetationVertexInputAttributeDescriptions(vegetationVertexInputAttributeDescriptions);
		vegetationPipelineCreationParameters.vertexInputAttributeDescriptionCount = static_cast<uint32>(vegetationVertexInputAttributeDescriptions.Size());
		vegetationPipelineCreationParameters.vertexInputAttributeDescriptions = vegetationVertexInputAttributeDescriptions.Data();
		VkVertexInputBindingDescription vegetationVertexInputBindingDescription;
		VulkanTranslationUtilities::GetVegetationVertexInputBindingDescription(vegetationVertexInputBindingDescription);
		vegetationPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
		vegetationPipelineCreationParameters.vertexInputBindingDescriptions = &vegetationVertexInputBindingDescription;
		vegetationPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::Vegetation)] = VulkanInterface::Instance->CreatePipeline(vegetationPipelineCreationParameters);
	}

	{
		//Create the lighting pipeline.
		VulkanPipelineCreationParameters lightingPipelineCreationParameters;

		lightingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingPipelineCreationParameters.blendEnable = false;
		lightingPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		lightingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		lightingPipelineCreationParameters.colorAttachments[0].Reserve(2);
		lightingPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::Scene)]->GetImageView());
		lightingPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::WaterScene)]->GetImageView());
		lightingPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		lightingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		lightingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		lightingPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		lightingPipelineCreationParameters.depthTestEnable = VK_FALSE;
		lightingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
		StaticArray<VulkanDescriptorSetLayout, 3> lightingDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Environment)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)]
		};
		lightingPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(lightingDescriptorSetLayouts.Size());
		lightingPipelineCreationParameters.descriptorSetLayouts = lightingDescriptorSetLayouts.Data();
		lightingPipelineCreationParameters.pushConstantRangeCount = 0;
		lightingPipelineCreationParameters.pushConstantRanges = nullptr;
		lightingPipelineCreationParameters.shaderModules.Reserve(2);
		lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
		lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::LightingFragmentShader)]);
		lightingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		lightingPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		lightingPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		lightingPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		lightingPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		lightingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::Lighting)] = VulkanInterface::Instance->CreatePipeline(lightingPipelineCreationParameters);
	}

	{
		//Create the cube map pipeline.
		VulkanPipelineCreationParameters cubeMapPipelineCreationParameters;

		cubeMapPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		cubeMapPipelineCreationParameters.blendEnable = false;
		cubeMapPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		cubeMapPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		cubeMapPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		cubeMapPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		cubeMapPipelineCreationParameters.colorAttachments[0].Reserve(2);
		cubeMapPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::Scene)]->GetImageView());
		cubeMapPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::WaterScene)]->GetImageView());
		cubeMapPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		cubeMapPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		cubeMapPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		cubeMapPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		cubeMapPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		cubeMapPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		cubeMapPipelineCreationParameters.depthTestEnable = VK_TRUE;
		cubeMapPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 2> cubeMapDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Environment)]
		};
		cubeMapPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(cubeMapDescriptorSetLayouts.Size());
		cubeMapPipelineCreationParameters.descriptorSetLayouts = cubeMapDescriptorSetLayouts.Data();
		cubeMapPipelineCreationParameters.pushConstantRangeCount = 0;
		cubeMapPipelineCreationParameters.pushConstantRanges = nullptr;
		cubeMapPipelineCreationParameters.shaderModules.Reserve(2);
		cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::CubeMapVertexShader)]);
		cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::CubeMapFragmentShader)]);
		cubeMapPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		cubeMapPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		cubeMapPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		cubeMapPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		cubeMapPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		cubeMapPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::CubeMap)] = VulkanInterface::Instance->CreatePipeline(cubeMapPipelineCreationParameters);
	}

	{
		//Create the ocean pipeline.
		VulkanPipelineCreationParameters oceanPipelineCreationParameters;

		oceanPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		oceanPipelineCreationParameters.blendEnable = false;
		oceanPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		oceanPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		oceanPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		oceanPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		oceanPipelineCreationParameters.colorAttachments[0].Reserve(1);
		oceanPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::Scene)]->GetImageView());
		oceanPipelineCreationParameters.cullMode = VK_CULL_MODE_BACK_BIT;
		oceanPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		oceanPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		oceanPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		oceanPipelineCreationParameters.depthBuffer = nullptr;
		oceanPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		oceanPipelineCreationParameters.depthTestEnable = VK_FALSE;
		oceanPipelineCreationParameters.depthWriteEnable = VK_FALSE;
		StaticArray<VulkanDescriptorSetLayout, 3> oceanDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Environment)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::Ocean)]
		};
		oceanPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(oceanDescriptorSetLayouts.Size());
		oceanPipelineCreationParameters.descriptorSetLayouts = oceanDescriptorSetLayouts.Data();
		oceanPipelineCreationParameters.pushConstantRangeCount = 0;
		oceanPipelineCreationParameters.pushConstantRanges = nullptr;
		oceanPipelineCreationParameters.shaderModules.Reserve(2);
		oceanPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
		oceanPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::OceanFragmentShader)]);
		oceanPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		oceanPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		oceanPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		oceanPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		oceanPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		oceanPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::Ocean)] = VulkanInterface::Instance->CreatePipeline(oceanPipelineCreationParameters);
	}

	{
		//Create the particle system pipeline.
		VulkanPipelineCreationParameters particleSystemPipelineCreationParameters;

		particleSystemPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
		particleSystemPipelineCreationParameters.blendEnable = true;
		particleSystemPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		particleSystemPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		particleSystemPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		particleSystemPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
		particleSystemPipelineCreationParameters.colorAttachments[0].Reserve(2);
		particleSystemPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::Scene)]->GetImageView());
		particleSystemPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::WaterScene)]->GetImageView());
		particleSystemPipelineCreationParameters.cullMode = VK_CULL_MODE_FRONT_BIT;
		particleSystemPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		particleSystemPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		particleSystemPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		particleSystemPipelineCreationParameters.depthBuffer = depthBuffers[INDEX(DepthBuffer::SceneBuffer)];
		particleSystemPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		particleSystemPipelineCreationParameters.depthTestEnable = VK_TRUE;
		particleSystemPipelineCreationParameters.depthWriteEnable = VK_FALSE;
		StaticArray<VulkanDescriptorSetLayout, 2> particleSystemDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::ParticleSystem)]
		};
		particleSystemPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(particleSystemDescriptorSetLayouts.Size());
		particleSystemPipelineCreationParameters.descriptorSetLayouts = particleSystemDescriptorSetLayouts.Data();
		VkPushConstantRange particleSystemPushConstantRange{ VK_SHADER_STAGE_GEOMETRY_BIT, 0, sizeof(float) };
		particleSystemPipelineCreationParameters.pushConstantRangeCount = 1;
		particleSystemPipelineCreationParameters.pushConstantRanges = &particleSystemPushConstantRange;
		particleSystemPipelineCreationParameters.shaderModules.Reserve(3);
		particleSystemPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ParticleSystemVertexShader)]);
		particleSystemPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ParticleSystemGeometryShader)]);
		particleSystemPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ParticleSystemFragmentShader)]);
		particleSystemPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		particleSystemPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		particleSystemPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		particleSystemPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		particleSystemPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		particleSystemPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::ParticleSystem)] = VulkanInterface::Instance->CreatePipeline(particleSystemPipelineCreationParameters);
	}

	{
		//Create the post processing pipeline.
		VulkanPipelineCreationParameters postProcessingPipelineCreationParameters;

		postProcessingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
		postProcessingPipelineCreationParameters.blendEnable = false;
		postProcessingPipelineCreationParameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
		postProcessingPipelineCreationParameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
		postProcessingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		const DynamicArray<VkImageView> &swapchainImageViews{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		const uint64 swapchainImageViewsSize{ swapchainImageViews.Size() };

		postProcessingPipelineCreationParameters.colorAttachments.UpsizeSlow(swapchainImageViewsSize);

		for (uint64 i = 0; i < swapchainImageViewsSize; ++i)
		{
			postProcessingPipelineCreationParameters.colorAttachments[i].Reserve(1);
			postProcessingPipelineCreationParameters.colorAttachments[i].EmplaceFast(swapchainImageViews[i]);
		}

		postProcessingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		postProcessingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		postProcessingPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		postProcessingPipelineCreationParameters.depthBuffer = nullptr;
		postProcessingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		postProcessingPipelineCreationParameters.depthTestEnable = VK_FALSE;
		postProcessingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
		StaticArray<VulkanDescriptorSetLayout, 2> postProcessingDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
			descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)]
		};
		postProcessingPipelineCreationParameters.descriptorSetLayoutCount = static_cast<uint32>(postProcessingDescriptorSetLayouts.Size());
		postProcessingPipelineCreationParameters.descriptorSetLayouts = postProcessingDescriptorSetLayouts.Data();
		postProcessingPipelineCreationParameters.pushConstantRangeCount = 0;
		postProcessingPipelineCreationParameters.pushConstantRanges = nullptr;
		postProcessingPipelineCreationParameters.shaderModules.Reserve(2);
		postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
		postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::PostProcessingFragmentShader)]);
		postProcessingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		postProcessingPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		postProcessingPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		postProcessingPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		postProcessingPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		postProcessingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

		pipelines[INDEX(Pipeline::PostProcessing)] = VulkanInterface::Instance->CreatePipeline(postProcessingPipelineCreationParameters);
	}
}

/*
*	Initializes all descriptor sets.
*/
void VulkanRenderingSystem::InitializeDescriptorSets() NOEXCEPT
{
	{
		//Initialize the shadow map blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(DescriptorSet::ShadowMapBlur)], descriptorSetLayouts[INDEX(DescriptorSetLayout::ShadowMapBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::DirectionalPreBlurShadowMap)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::ShadowMapBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the lighting descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 4> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 1),
			renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 2),
			renderTargets[INDEX(RenderTarget::DirectionalPostBlurShadowMap)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 3)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(DescriptorSet::PostProcessing)], descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 2> writeDescriptorSets
		{
			uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::PostProcessing)], 1),
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::PostProcessing)], 2)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Initializes all default textures.
*/
void VulkanRenderingSystem::InitializeDefaultTextures() NOEXCEPT
{
	byte defaultMetallic[]{ 0 };
	defaultTextures[DefaultTexture::Black] = static_cast<Vulkan2DTexture *RESTRICT>(Create2DTexture(TextureData(TextureDataContainer(defaultMetallic, 1, 1, 1), AddressMode::Repeat, TextureFilter::Nearest, MipmapMode::Nearest, TextureFormat::R8_Byte)));

	byte defaultRoughness[]{ 255 };
	defaultTextures[DefaultTexture::White] = static_cast<Vulkan2DTexture *RESTRICT>(Create2DTexture(TextureData(TextureDataContainer(defaultRoughness, 1, 1, 1), AddressMode::Repeat, TextureFilter::Nearest, MipmapMode::Nearest, TextureFormat::R8_Byte)));
}

/*
*	Calculates the projection matrix.
*/
void VulkanRenderingSystem::CalculateProjectionMatrix() NOEXCEPT
{
	//Calculate the projection matrix.
	projectionMatrix = Matrix4::Perspective(CatalystMath::DegreesToRadians(activeCamera->GetFieldOfView()), 1920.0f / 1080.0f, activeCamera->GetNearPlane(), activeCamera->GetFarPlane());
}

/*
*	Execute asynchronous tasks.
*/
void VulkanRenderingSystem::ExecuteAsynchronousTasks() NOEXCEPT
{
	//Execute the asynchronous tasks.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateParticleSystemProperties();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateParticleSystemProperties)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateVegetationCulling();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateVegetationCulling)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateViewFrustumCulling();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateViewFrustumCuling)]));
}

/*
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Set the current frame.
	frameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Set the current command buffer.
	currentCommandBuffer = frameData.GetCurrentPrimaryCommandBuffer();

	//Set the current dynamic uniform data descriptor set.
	currentDynamicUniformDataDescriptorSet = frameData.GetCurrentDynamicUniformDataDescriptorSet();

	//Set the current environment descriptor set.
	currentEnvironmentDataDescriptorSet = frameData.GetCurrentEnvironmentDescriptorSet();

	//Wait for the current fence to finish.
	frameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	frameData.GetCurrentFence()->Reset();
}

/*
*	Executes frame-dependant asynchronous tasks.
*/
void VulkanRenderingSystem::ExecuteFrameDependantAsynchronousTasks() NOEXCEPT
{
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderDirectionalShadows();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderDirectionalShadows)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderTerrain();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderTerrain)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderStaticPhysicalEntities();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderStaticPhysicalEntities)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderInstancedPhysicalEntities();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderInstancedPhysicalEntities)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderVegetationEntities();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderVegetationEntities)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderLighting();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderLighting)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderSkybox();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderSkybox)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderParticleSystemEntities();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderParticleSystemEntities)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderOcean();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderOcean)]));

	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		VulkanRenderingSystem::Instance->RenderPostProcessing();
	}, nullptr, &taskSemaphores[INDEX(TaskSemaphore::RenderPostProcessing)]));
}

/*
*	Concatenates all secondary command buffers into the previous one.
*/
void VulkanRenderingSystem::ConcatenateCommandBuffers() NOEXCEPT
{
	//Begin up the primary command buffer.
	currentCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Begin the terrain render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<4>(pipelines[INDEX(Pipeline::Terrain)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Wait for the render terrain task to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderTerrain)].WaitFor();

	//Record the execute command for the terrain.
	currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentTerrainCommandBuffer()->Get());

	//End the terrain render pass.
	currentCommandBuffer->CommandEndRenderPass();

	if (ComponentManager::GetNumberOfStaticPhysicalComponents() > 0)
	{
		//Begin the static physical entities render pass.
		currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::StaticPhysical)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Wait for the render static physical entities task to finish.
		taskSemaphores[INDEX(TaskSemaphore::RenderStaticPhysicalEntities)].WaitFor();

		//Record the execute command for the static physical entities.
		currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentStaticPhysicalEntitiesCommandBuffer()->Get());

		//End the static physical entities render pass.
		currentCommandBuffer->CommandEndRenderPass();
	}

	if (ComponentManager::GetNumberOfInstancedPhysicalComponents() > 0)
	{
		//Begin the instanced physical entities render pass.
		currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::InstancedPhysical)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	
		//Wait for the render instanced physical entities task to finish.
		taskSemaphores[INDEX(TaskSemaphore::RenderInstancedPhysicalEntities)].WaitFor();

		//Record the execute command for the instanced physical entities.
		currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentInstancedPhysicalEntitiesCommandBuffer()->Get());

		//End the instanced physical entities render pass.
		currentCommandBuffer->CommandEndRenderPass();
	}

	if (ComponentManager::GetNumberOfVegetationComponents() > 0)
	{
		//Begin the vegetation entities render pass.
		currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::Vegetation)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Wait for the render vegetation entities task to finish.
		taskSemaphores[INDEX(TaskSemaphore::RenderVegetationEntities)].WaitFor();

		//Record the execute command for the vegetation entities.
		currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentVegetationEntitiesCommandBuffer()->Get());

		//End the vegetation entities render pass.
		currentCommandBuffer->CommandEndRenderPass();
	}

	//Begin the lighting render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::Lighting)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Wait for the render lighting task to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderLighting)].WaitFor();

	//Record the execute command for the lighting.
	currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentLightingCommandBuffer()->Get());

	//End the lighting render pass.
	currentCommandBuffer->CommandEndRenderPass();

	//Reset the directional shadow event.
	currentCommandBuffer->CommandResetEvent(frameData.GetCurrentDirectionalShadowEvent()->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	//Begin the skybox render pass.
	currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::CubeMap)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Wait for the render skybox task to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderSkybox)].WaitFor();

	//Record the execute command for the skybox.
	currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentSkyboxCommandBuffer()->Get());

	//End the skybox render pass.
	currentCommandBuffer->CommandEndRenderPass();

	if (ComponentManager::GetNumberOfParticleSystemComponents() > 0)
	{
		//Begin the particle system entities render pass.
		currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::ParticleSystem)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Wait for the render particle system entities task to finish.
		taskSemaphores[INDEX(TaskSemaphore::RenderParticleSystemEntities)].WaitFor();

		//Record the execute command for the particle system entities.
		currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentParticleSystemEntitiesCommandBuffer()->Get());

		//End the static particle system render pass.
		currentCommandBuffer->CommandEndRenderPass();
	}

	//Bind the ocean render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::Ocean)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Wait for the render ocean task to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderOcean)].WaitFor();

	//Record the execute command for the ocean.
	currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentOceanCommandBuffer()->Get());

	//End the static particle system render pass.
	currentCommandBuffer->CommandEndRenderPass();

	//Bind the post processing render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::PostProcessing)]->GetRenderPass(), frameData.GetCurrentFrame(), VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Wait for the render post processing task to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderPostProcessing)].WaitFor();

	//Record the execute command for the post processing.
	currentCommandBuffer->CommandExecuteCommands(frameData.GetCurrentPostProcessingCommandBuffer()->Get());

	//End the post processing render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Ends the frame.
*/
void VulkanRenderingSystem::EndFrame() NOEXCEPT
{
	//End the current command buffer.
	currentCommandBuffer->End();

	//Wait for the frame-dependant asynchonous tasks to finish.
	taskSemaphores[INDEX(TaskSemaphore::RenderDirectionalShadows)].WaitFor();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(*currentCommandBuffer, 1, semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, semaphores[INDEX(GraphicsSemaphore::RenderFinished)], frameData.GetCurrentFence()->Get());
}

/*
*	Updates the properties of particle systems.
*/
void VulkanRenderingSystem::UpdateParticleSystemProperties() const NOEXCEPT
{
	//Go through every particle system component and re-upload the properties, since they might change.
	const uint64 numberOfParticleSystemComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there are none, just return.
	if (numberOfParticleSystemComponents == 0)
	{
		return;
	}

	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemComponents() };

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		static_cast<VulkanUniformBuffer *const RESTRICT>(component->propertiesUniformBuffer)->UploadData(&VulkanParticleSystemProperties(component->properties));
	}
}

/*
*	Updates the vegetation culling.
*/
void VulkanRenderingSystem::UpdateVegetationCulling() const NOEXCEPT
{
	//Go through all vegetation components and update their culling
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there are none, just return.
	if (numberOfVegetationComponents == 0)
	{
		return;
	}

	VegetationComponent *RESTRICT renderComponent{ ComponentManager::GetVegetationComponents() };
	const VegetationCullingComponent *RESTRICT cullingComponent{ ComponentManager::GetVegetationCullingComponents() };

	const Vector3 &cameraWorldPosition{ activeCamera->GetPosition() };

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++renderComponent, ++cullingComponent)
	{
		for (uint64 i = 0, size = renderComponent->shouldDrawGridCell.Size(); i < size; ++i)
		{
			renderComponent->shouldDrawGridCell[i] =	CatalystMath::Absolute(cameraWorldPosition.X - cullingComponent->gridCellCenterLocations[i].X) <= cullingComponent->cutoffDistance &&
														CatalystMath::Absolute(cameraWorldPosition.Z - cullingComponent->gridCellCenterLocations[i].Y) <= cullingComponent->cutoffDistance;
		}
	}
}

/*
*	Updates the view frustum culling.
*/
void VulkanRenderingSystem::UpdateViewFrustumCulling() NOEXCEPT
{
	//Calulate the view matrix.
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	//Iterate over all static physical entity components to check if they are in the view frustum.
	const uint64 numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };
	FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents() };

	for (uint64 i = 0; i < numberOfPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++transformComponent)
	{
		//Make a local copy of the static physical entity's position.
		const Vector3 position = transformComponent->position;
		const Vector3 scale = transformComponent->scale;
		const float biggestScale = CatalystMath::Maximum(scale.X, CatalystMath::Maximum(scale.Y, scale.Z));
		const float scaledExtent = frustumCullingComponent->axisAlignedBoundingBox.maximum.X * biggestScale;

		Vector4 corners[8];

		corners[0] = Vector4(-scaledExtent, -scaledExtent, -scaledExtent, 1.0f);
		corners[1] = Vector4(-scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[2] = Vector4(scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[3] = Vector4(scaledExtent, -scaledExtent, -scaledExtent, 1.0f);

		corners[4] = Vector4(-scaledExtent, -scaledExtent, scaledExtent, 1.0f);
		corners[5] = Vector4(-scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[6] = Vector4(scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[7] = Vector4(scaledExtent, -scaledExtent, scaledExtent, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] += Vector4(position.X, position.Y, position.Z, 0.0f);

			corners[i] = viewMatrix * corners[i];

			corners[i].X /= corners[i].W;
			corners[i].Y /= corners[i].W;
			corners[i].Z /= corners[i].W;
		}

		frustumCullingComponent->isInViewFrustum = RenderingUtilities::IsCubeWithinViewFrustum(corners);
	}
}

/*
*	Renders directional shadows.
*/
void VulkanRenderingSystem::RenderDirectionalShadows() NOEXCEPT
{
	//Cache the current directional shadow command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentDirectionalShadowCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Render terrain shadows.
	{
		//Cache the pipeline.
		VulkanPipeline &directionalShadowTerrainPipeline{ *pipelines[INDEX(Pipeline::DirectionalShadowTerrain)] };

		//Begin the pipeline and render pass.
		commandBuffer->CommandBindPipeline(directionalShadowTerrainPipeline);
		commandBuffer->CommandBeginRenderPassAndClear<2>(directionalShadowTerrainPipeline.GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_INLINE);

		//Iterate over all terrain entity components and draw them all.
		const uint64 numberOfTerrainComponents{ ComponentManager::GetNumberOfTerrainComponents() };
		const TerrainRenderComponent *RESTRICT renderComponent{ ComponentManager::GetTerrainRenderComponents() };

		for (uint64 i = 0; i < numberOfTerrainComponents; ++i, ++renderComponent)
		{
			StaticArray<VulkanDescriptorSet, 2> terrainDescriptorSets
			{
				*currentDynamicUniformDataDescriptorSet,
				renderComponent->descriptorSet
			};

			const VkDeviceSize offset{ 0 };

			commandBuffer->CommandBindDescriptorSets(directionalShadowTerrainPipeline, static_cast<uint32>(terrainDescriptorSets.Size()), terrainDescriptorSets.Data());
			commandBuffer->CommandBindVertexBuffers(1, &static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->vertexAndIndexBuffer)->Get(), &offset);
			commandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->vertexAndIndexBuffer), renderComponent->indexBufferOffset);
			commandBuffer->CommandDrawIndexed(renderComponent->indexCount, 1);
		}

		//End the render pass.
		commandBuffer->CommandEndRenderPass();
	}

	//Render instanced physical shadows.
	{
		//Iterate over all instanced physical entity components and draw them all.
		const uint64 numberOfInstancedPhysicalComponents{ ComponentManager::GetNumberOfInstancedPhysicalComponents() };

		//If there's none to draw - draw none!
		if (numberOfInstancedPhysicalComponents > 0)
		{
			//Cache the pipeline.
			VulkanPipeline &directionalShadowInstancedPhysicalPipeline{ *pipelines[INDEX(Pipeline::DirectionalShadowInstancedPhysical)] };

			const InstancedPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetInstancedPhysicalRenderComponents() };

			//Begin the pipeline and render pass.
			commandBuffer->CommandBindPipeline(directionalShadowInstancedPhysicalPipeline);
			commandBuffer->CommandBeginRenderPass(directionalShadowInstancedPhysicalPipeline.GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_INLINE);

			for (uint64 i = 0; i < numberOfInstancedPhysicalComponents; ++i, ++renderComponent)
			{
				StaticArray<VulkanDescriptorSet, 2> instancedPhysicalDescriptorSets
				{
					*currentDynamicUniformDataDescriptorSet,
					renderComponent->descriptorSet
				};

				StaticArray<VkBuffer, 2> instancedPhysicalBuffers
				{
					static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->modelBuffer)->Get(),
					static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->transformationsBuffer)->Get()
				};

				StaticArray<VkDeviceSize, 2> offsets
				{
					static_cast<VkDeviceSize>(0),
					static_cast<VkDeviceSize>(0)
				};

				commandBuffer->CommandBindDescriptorSets(directionalShadowInstancedPhysicalPipeline, 2, instancedPhysicalDescriptorSets.Data());
				commandBuffer->CommandBindVertexBuffers(2, instancedPhysicalBuffers.Data(), offsets.Data());
				commandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->modelBuffer), renderComponent->indexOffset);
				commandBuffer->CommandDrawIndexed(renderComponent->indexCount, renderComponent->instanceCount);
			}

			//End the render pass.
			commandBuffer->CommandEndRenderPass();
		}
	}

	//Blur the directional shadow map.
	{
		//Bind the shadow map blur pipeline.
		commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::ShadowMapBlur)]);

		//Bind the shadow map blur render pass.
		commandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::ShadowMapBlur)]->GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_INLINE);

		//Bind the shadow map blur descriptor set.
		StaticArray<VulkanDescriptorSet, 2> shadowMapBlurDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			descriptorSets[INDEX(DescriptorSet::ShadowMapBlur)]
		};

		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::ShadowMapBlur)], static_cast<uint32>(shadowMapBlurDescriptorSets.Size()), shadowMapBlurDescriptorSets.Data());

		//Draw the viewport!
		commandBuffer->CommandDraw(4, 1);

		//End the shadow map blur render pass.
		commandBuffer->CommandEndRenderPass();
	}

	//Signal the directional shadow event.
	commandBuffer->CommandSetEvent(frameData.GetCurrentDirectionalShadowEvent()->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	//End the command buffer.
	commandBuffer->End();

	//Submit the directional shadow command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(*commandBuffer, 0, nullptr, 0, 0, nullptr, nullptr);
}

/*
*	Renders the terrain.
*/
void VulkanRenderingSystem::RenderTerrain() NOEXCEPT
{
	//Cache the current terrain command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentTerrainCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::Terrain)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::Terrain)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::Terrain)]);

	//Iterate over all terrain entity components and draw them all.
	const uint64 numberOfTerrainEntityComponents{ ComponentManager::GetNumberOfTerrainComponents() };
	const TerrainRenderComponent *RESTRICT terrainRenderComponent{ ComponentManager::GetTerrainRenderComponents() };

	for (uint64 i = 0; i < numberOfTerrainEntityComponents; ++i, ++terrainRenderComponent)
	{
		StaticArray<VulkanDescriptorSet, 2> terrainDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			terrainRenderComponent->descriptorSet
		};

		const VkDeviceSize offset{ 0 };

		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::Terrain)], 2, terrainDescriptorSets.Data());
		commandBuffer->CommandBindVertexBuffers(1, &static_cast<const VulkanConstantBuffer *RESTRICT>(terrainRenderComponent->vertexAndIndexBuffer)->Get(), &offset);
		commandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanConstantBuffer *RESTRICT>(terrainRenderComponent->vertexAndIndexBuffer), terrainRenderComponent->indexBufferOffset);
		commandBuffer->CommandDrawIndexed(terrainRenderComponent->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders all static physical entities.
*/
void VulkanRenderingSystem::RenderStaticPhysicalEntities() NOEXCEPT
{
	//Wait for the update view frustum culling  task to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateViewFrustumCuling)].WaitFor();


	//Iterate over all static physical entity components and draw them all.
	const uint64 numberOfStaticPhysicalEntityComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };

	if (numberOfStaticPhysicalEntityComponents == 0)
	{
		return;
	}

	const FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents() };
	const StaticPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents() };

	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentStaticPhysicalEntitiesCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::StaticPhysical)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::StaticPhysical)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Begin the pipeline and render pass.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::StaticPhysical)]);

	for (uint64 i = 0; i < numberOfStaticPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++renderComponent)
	{
		//Don't draw this static physical entity if it isn't in the view frustum.
		if (!frustumCullingComponent->isInViewFrustum)
		{
			continue;
		}

		StaticArray<VulkanDescriptorSet, 2> staticPhysicalEntityDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			renderComponent->descriptorSet
		};

		const VkDeviceSize offset{ 0 };

		commandBuffer->CommandPushConstants(pipelines[INDEX(Pipeline::StaticPhysical)]->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4), &renderComponent->modelMatrix);
		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::StaticPhysical)], 2, staticPhysicalEntityDescriptorSets.Data());
		commandBuffer->CommandBindVertexBuffers(1, &renderComponent->buffer.Get(), &offset);
		commandBuffer->CommandBindIndexBuffer(renderComponent->buffer, renderComponent->indexOffset);
		commandBuffer->CommandDrawIndexed(renderComponent->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders all instanced physical entities.
*/
void VulkanRenderingSystem::RenderInstancedPhysicalEntities() NOEXCEPT
{
	//Iterate over all instanced physical entity components and draw them all.
	const uint64 numberOfInstancedPhysicalComponents{ ComponentManager::GetNumberOfInstancedPhysicalComponents() };

	//If there's none to draw - draw none!
	if (numberOfInstancedPhysicalComponents == 0)
	{
		return;
	}

	const InstancedPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetInstancedPhysicalRenderComponents() };

	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentInstancedPhysicalEntitiesCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::InstancedPhysical)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::InstancedPhysical)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::InstancedPhysical)]);
	
	for (uint64 i = 0; i < numberOfInstancedPhysicalComponents; ++i, ++renderComponent)
	{
		StaticArray<VulkanDescriptorSet, 2> instancedPhysicalDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			renderComponent->descriptorSet
		};

		StaticArray<VkBuffer, 2> instancedPhysicalBuffers
		{
			static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->modelBuffer)->Get(),
			static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->transformationsBuffer)->Get()
		};

		StaticArray<VkDeviceSize, 2> offsets
		{
			static_cast<VkDeviceSize>(0),
			static_cast<VkDeviceSize>(0)
		};

		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::InstancedPhysical)], 2, instancedPhysicalDescriptorSets.Data());
		commandBuffer->CommandBindVertexBuffers(2, instancedPhysicalBuffers.Data(), offsets.Data());
		commandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanConstantBuffer *RESTRICT>(renderComponent->modelBuffer), renderComponent->indexOffset);
		commandBuffer->CommandDrawIndexed(renderComponent->indexCount, renderComponent->instanceCount);
	}

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders all vegetation entities.
*/
void VulkanRenderingSystem::RenderVegetationEntities() NOEXCEPT
{
	//Iterate over all vegetation entity components and draw them all.
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there's none to draw - draw none!
	if (numberOfVegetationComponents == 0)
	{
		return;
	}

	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationComponents() };

	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentVegetationEntitiesCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::Vegetation)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::Vegetation)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::Vegetation)]);

	//Wait for the vegetation culling task to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateVegetationCulling)].WaitFor();

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		StaticArray<VulkanDescriptorSet, 2> vegetationDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			component->descriptorSet
		};

		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::Vegetation)], static_cast<uint32>(vegetationDescriptorSets.Size()), vegetationDescriptorSets.Data());

		for (uint64 j = 0, size = component->shouldDrawGridCell.Size(); j < size; ++j)
		{
			if (component->shouldDrawGridCell[j])
			{
				commandBuffer->CommandBindVertexBuffers(1, &static_cast<const VulkanConstantBuffer *RESTRICT>(component->transformationsBuffer)->Get(), &component->transformationOffsets[j]);
				commandBuffer->CommandDraw(1, component->instanceCounts[j]);
			}
		}
	}

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders lighting.
*/
void VulkanRenderingSystem::RenderLighting() NOEXCEPT
{
	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentLightingCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::Lighting)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::Lighting)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::Lighting)]);

	//Bind the descriptor sets.
	StaticArray<VulkanDescriptorSet, 3> lightingDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		*currentEnvironmentDataDescriptorSet,
		descriptorSets[INDEX(DescriptorSet::Lighting)]
	};

	commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::Lighting)], static_cast<uint32>(lightingDescriptorSets.Size()), lightingDescriptorSets.Data());

	//Wait for the directional shadows to finish.
	commandBuffer->CommandWaitEvents(1, &frameData.GetCurrentDirectionalShadowEvent()->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

	//Draw the viewport!
	commandBuffer->CommandDraw(4, 1);

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders skybox.
*/
void VulkanRenderingSystem::RenderSkybox() NOEXCEPT
{
	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentSkyboxCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::CubeMap)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::CubeMap)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::CubeMap)]);

	//Bind the skybox descriptor set.
	StaticArray<VulkanDescriptorSet, 2> skyboxDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		*currentEnvironmentDataDescriptorSet
	};

	commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::CubeMap)], static_cast<uint32>(skyboxDescriptorSets.Size()), skyboxDescriptorSets.Data());

	//Draw the skybox!
	commandBuffer->CommandDraw(36, 1);

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders all particle system entities.
*/
void VulkanRenderingSystem::RenderParticleSystemEntities() NOEXCEPT
{
	//Iterate over all particle system entity components and draw them all.
	const uint64 numberOfParticleSystemComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to draw - draw none!
	if (numberOfParticleSystemComponents == 0)
	{
		return;
	}

	//Wait for the update particle system properties task to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateParticleSystemProperties)].WaitFor();

	//Iterate over all components.
	const ParticleSystemRenderComponent *RESTRICT component{ ComponentManager::GetParticleSystemRenderComponents() };

	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentParticleSystemEntitiesCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::ParticleSystem)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::ParticleSystem)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::ParticleSystem)]);

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		StaticArray<VulkanDescriptorSet, 2> particleSystemDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			component->descriptorSet
		};

		const float randomSeed{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
		commandBuffer->CommandPushConstants(pipelines[INDEX(Pipeline::ParticleSystem)]->GetPipelineLayout(), VK_SHADER_STAGE_GEOMETRY_BIT, 0, sizeof(float), &randomSeed);
		commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::ParticleSystem)], static_cast<uint32>(particleSystemDescriptorSets.Size()), particleSystemDescriptorSets.Data());
		commandBuffer->CommandDraw(VulkanRenderingSystemConstants::MAXIMUM_NUMBER_OF_PARTICLES, 1);
	}

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders the ocean.
*/
void VulkanRenderingSystem::RenderOcean() NOEXCEPT
{
	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentOceanCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::Ocean)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::Ocean)]->GetRenderPass().GetFrameBuffers()[0].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::Ocean)]);

	//Bind the ocean descriptor sets.
	StaticArray<VulkanDescriptorSet, 3> oceanDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		*currentEnvironmentDataDescriptorSet,
		*frameData.GetCurrentOceanDescriptorSet()
	};

	commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::Ocean)], static_cast<uint32>(oceanDescriptorSets.Size()), oceanDescriptorSets.Data());

	//Draw the viewport!
	commandBuffer->CommandDraw(4, 1);

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Renders the post processing.
*/
void VulkanRenderingSystem::RenderPostProcessing() NOEXCEPT
{
	//Cache the command buffer.
	VulkanCommandBuffer *const RESTRICT commandBuffer{ frameData.GetCurrentPostProcessingCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelines[INDEX(Pipeline::PostProcessing)]->GetRenderPass().Get(), pipelines[INDEX(Pipeline::PostProcessing)]->GetRenderPass().GetFrameBuffers()[frameData.GetCurrentFrame()].Get());

	//Bind the pipeline.
	commandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::PostProcessing)]);

	//Bind the post processing descriptor sets.
	StaticArray<VulkanDescriptorSet, 2> postProcessingDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		descriptorSets[INDEX(DescriptorSet::PostProcessing)]
	};

	commandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::PostProcessing)], 2, postProcessingDescriptorSets.Data());

	//Draw the viewport!
	commandBuffer->CommandDraw(4, 1);

	//End the command buffer.
	commandBuffer->End();
}

/*
*	Updates the descriptor sets.
*/
void VulkanRenderingSystem::UpdateDescriptorSets() NOEXCEPT
{
	{
		//Update the environment descriptor set.
		VulkanDescriptorSet &environmentDescriptorSet{ *currentEnvironmentDataDescriptorSet };

		StaticArray<VkWriteDescriptorSet, 4> environmentWriteDescriptorSets
		{
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentSystem::Instance->GetNightEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 0),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentSystem::Instance->GetNightEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentSystem::Instance->GetDayEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 2),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentSystem::Instance->GetDayEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 3)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(environmentWriteDescriptorSets.Size()), environmentWriteDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Update the ocean descriptor set.
		VulkanDescriptorSet &oceanDescriptorSet{ *frameData.GetCurrentOceanDescriptorSet() };

		StaticArray<VkWriteDescriptorSet, 3> oceanWriteDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::WaterScene)]->GetWriteDescriptorSet(oceanDescriptorSet, 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetWriteDescriptorSet(oceanDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentSystem::Instance->GetOceanMaterial().normalMapTexture)->GetWriteDescriptorSet(oceanDescriptorSet, 2)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(oceanWriteDescriptorSets.Size()), oceanWriteDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Updates the dynamic uniform data.
*/
void VulkanRenderingSystem::UpdateDynamicUniformData() NOEXCEPT
{
	//Calculate the camera data
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	Matrix4 inverseCameraMatrix{ cameraMatrix };
	inverseCameraMatrix.Inverse();

	Matrix4 cameraOriginMatrix{ cameraMatrix };
	cameraOriginMatrix.SetTranslation(Vector3(0.0f, 0.0f, 0.0f));

	Matrix4 inverseProjectionMatrix{ projectionMatrix };
	inverseProjectionMatrix.Inverse();

	dynamicUniformData.cameraFieldOfViewCosine = CatalystMath::CosineDegrees(activeCamera->GetFieldOfView()) - 0.2f;
	dynamicUniformData.inverseCameraMatrix = inverseCameraMatrix;
	dynamicUniformData.inverseProjectionMatrix = inverseProjectionMatrix;
	dynamicUniformData.originViewMatrix = projectionMatrix * cameraOriginMatrix;
	dynamicUniformData.viewMatrix = viewMatrix;
	dynamicUniformData.cameraForwardVector = forwardVector;
	dynamicUniformData.cameraWorldPosition = cameraWorldPosition;

	const uint64 numberOfDirectionalLightEntityComponents{ ComponentManager::GetNumberOfDirectionalLightComponents() };

	if (numberOfDirectionalLightEntityComponents > 0)
	{
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightComponents() };

		dynamicUniformData.directionalLightIntensity = directionalLightComponent->intensity;
		dynamicUniformData.directionalLightViewMatrix = RenderingUtilities::CalculateDirectionalLightViewMatrix();
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->rotation);
		dynamicUniformData.directionalLightColor = directionalLightComponent->color;
		dynamicUniformData.directionalLightScreenSpacePosition = viewMatrix * Vector4(-dynamicUniformData.directionalLightDirection.X * 100.0f + cameraWorldPosition.X, -dynamicUniformData.directionalLightDirection.Y * 100.0f + cameraWorldPosition.Y, -dynamicUniformData.directionalLightDirection.Z * 100.0f + cameraWorldPosition.Z, 1.0f);
		dynamicUniformData.directionalLightScreenSpacePosition.X /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Y /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Z /= dynamicUniformData.directionalLightScreenSpacePosition.W;
	}

	else
	{
		dynamicUniformData.directionalLightIntensity = 0.0f;
		dynamicUniformData.directionalLightViewMatrix = Matrix4();
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightColor = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightScreenSpacePosition = Vector3(0.0f, 0.0f, 0.0f);
	}

	dynamicUniformData.environmentBlend = EnvironmentSystem::Instance->GetEnvironmentBlend();

	dynamicUniformData.deltaTime = EngineSystem::Instance->GetDeltaTime();
	dynamicUniformData.totalGameTime = EngineSystem::Instance->GetTotalGameTime();

	uint64 counter = 0;

	const uint64 numberOfPointLightEntityComponents{ ComponentManager::GetNumberOfPointLightComponents() };
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightComponents() };

	dynamicUniformData.numberOfPointLights = static_cast<int32>(numberOfPointLightEntityComponents);

	for (uint64 i = 0; i < numberOfPointLightEntityComponents; ++i, ++pointLightComponent)
	{
		if (!pointLightComponent->enabled)
		{
			--dynamicUniformData.numberOfPointLights;

			continue;
		}

		dynamicUniformData.pointLightAttenuationDistances[counter] = pointLightComponent->attenuationDistance;
		dynamicUniformData.pointLightIntensities[counter] = pointLightComponent->intensity;
		dynamicUniformData.pointLightColors[counter] = pointLightComponent->color;
		dynamicUniformData.pointLightWorldPositions[counter] = pointLightComponent->position;

		++counter;
	}

	counter = 0;

	const uint64 numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightComponents() };

	dynamicUniformData.numberOfSpotLights = static_cast<int32>(numberOfSpotLightEntityComponents);

	for (uint64 i = 0; i < numberOfSpotLightEntityComponents; ++i, ++spotLightComponent)
	{
		if (!spotLightComponent->enabled)
		{
			--dynamicUniformData.numberOfSpotLights;

			continue;
		}

		dynamicUniformData.spotLightAttenuationDistances[counter] = spotLightComponent->attenuationDistance;
		dynamicUniformData.spotLightIntensities[counter] = spotLightComponent->intensity;
		dynamicUniformData.spotLightInnerCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->innerCutoffAngle);
		dynamicUniformData.spotLightOuterCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->outerCutoffAngle);
		dynamicUniformData.spotLightColors[counter] = spotLightComponent->color;
		dynamicUniformData.spotLightDirections[counter] = Vector3(0.0f, -1.0f, 0.0f).Rotated(spotLightComponent->rotation);
		dynamicUniformData.spotLightDirections[counter].Y *= -1.0f;
		dynamicUniformData.spotLightWorldPositions[counter] = spotLightComponent->position;

		++counter;
	}

	//Update the physics data.
	dynamicUniformData.windStrength = PhysicsSystem::Instance->GetWindStrength();
	dynamicUniformData.windDirection = PhysicsSystem::Instance->GetWindDirection();

	//Upload the dynamic uniform data to the uniform buffer.
	frameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&dynamicUniformData));
}