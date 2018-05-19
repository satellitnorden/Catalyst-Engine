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
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/VegetationMaterial.h>
#include <Rendering/Engine Layer/Render Passes/RenderPasses.h>
#include <Rendering/Shader Data/Vulkan/VulkanShaderData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanParticleSystemProperties.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationCommandBuffer.h>
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
*	Pre-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Execute the asynchronous tasks.
	ExecuteAsynchronousTasks();

	//Update the main window.
	mainWindow.Update();

	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(semaphores[INDEX(GraphicsSemaphore::ImageAvailable)]);

	//Begin the frame.
	BeginFrame();

	//Update the dynamic uniform data.
	UpdateDynamicUniformData();

	//Update the descriptor sets.
	UpdateDescriptorSets();
}

/*
*	Post-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Concatenate all secondary command buffers into the previous one.
	ConcatenateCommandBuffers();

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[INDEX(GraphicsSemaphore::RenderFinished)]);

	//Check if the main window should close - If so, terminate.
	if (mainWindow.ShouldClose())
	{
		EngineSystem::Instance->Terminate();
	}
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
*	Returns the render resolution.
*/
Resolution VulkanRenderingSystem::GetRenderResolution() const NOEXCEPT
{
	//Return the render resolution.
	VkExtent2D swapExtent{ VulkanInterface::Instance->GetSwapchain().GetSwapExtent() };

	return Resolution(swapExtent.width, swapExtent.height);
}

/*
*	Returns the current frame index.
*/
uint8 VulkanRenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
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
	GraphicsBufferHandle buffer = VulkanInterface::Instance->CreateConstantBuffer(modelData, modelDataSizes, 2)->Get();

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
	VulkanDescriptorSet newDescriptorSet;
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Terrain)]);

	terrainRenderComponent.descriptorSet = newDescriptorSet.Get();

	DynamicArray<VkWriteDescriptorSet, 18> writeDescriptorSets;

	Texture2DHandle terrainPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(terrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	writeDescriptorSets.EmplaceFast(static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent.uniformBuffer)->GetWriteDescriptorSet(newDescriptorSet, 1));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 2));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(layerWeightsTexture)->GetWriteDescriptorSet(newDescriptorSet, 3));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 4));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 5));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.firstLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 6));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 7));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 8));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.secondLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 9));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 10));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 11));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.thirdLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 12));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 13));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 14));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fourthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 15));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 16));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 17));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainMaterial.fifthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 18));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	terrainRenderComponent.vertexAndIndexBuffer = terrainVertexBuffer->Get();
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
	renderComponent.descriptorSet = newDescriptorSet.Get();
	renderComponent.buffer = model.GetBuffer();
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

	renderComponent.descriptorSet = newDescriptorSet.Get();
	renderComponent.modelBuffer = model.GetBuffer();
	renderComponent.transformationsBuffer = transformationsBuffer->Get();
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
	renderComponent.descriptorSet = newDescriptorSet.Get();
	renderComponent.transformationsBuffer = transformationsBuffer->Get();
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
	renderComponent.descriptorSet = descriptorSet.Get();
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
*	Finalizes the initialization of a render pass.
*/
void VulkanRenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Create the pipeline.
	VulkanPipelineCreationParameters parameters;

	parameters.attachmentLoadOperator = VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(renderPass->GetColorAttachmentLoadOperator());
	parameters.blendEnable = renderPass->IsBlendEnabled();

	if (renderPass->GetRenderTargets()[0] == RenderTarget::Screen)
	{
		parameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
		parameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
		parameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		const DynamicArray<VkImageView> &swapchainImageViews{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		const uint64 swapchainImageViewsSize{ swapchainImageViews.Size() };

		parameters.colorAttachments.UpsizeSlow(swapchainImageViewsSize);

		for (uint64 i = 0; i < swapchainImageViewsSize; ++i)
		{
			parameters.colorAttachments[i].Reserve(1);
			parameters.colorAttachments[i].EmplaceFast(swapchainImageViews[i]);
		}
	}

	else
	{
		parameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		parameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		parameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		parameters.colorAttachments.UpsizeSlow(1);
		parameters.colorAttachments[0].Reserve(renderPass->GetRenderTargets().Size());

		for (const RenderTarget renderTarget : renderPass->GetRenderTargets())
		{
			parameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(renderTarget)]->GetImageView());
		}
	}

	parameters.cullMode = VulkanTranslationUtilities::GetVulkanCullMode(renderPass->GetCullMode());
	parameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	parameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	parameters.depthAttachmentStoreOp = VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(renderPass->GetDepthAttachmentStoreOperator());
	parameters.depthBuffer = renderPass->GetDepthBuffer() == DepthBuffer::None ? nullptr : depthBuffers[INDEX(renderPass->GetDepthBuffer())];
	parameters.depthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(renderPass->GetDepthCompareOperator());
	parameters.depthTestEnable = renderPass->IsDepthTestEnabled();
	parameters.depthWriteEnable = renderPass->IsDepthWriteEnabled();
	
	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(renderPass->GetDescriptorSetLayouts().Size());

	for (const DescriptorSetLayout descriptorSetLayout : renderPass->GetDescriptorSetLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(descriptorSetLayouts[INDEX(descriptorSetLayout)]);
	}

	parameters.descriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
	parameters.descriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

	DynamicArray<VkPushConstantRange> pushConstantRanges;

	if (renderPass->GetPushConstantRanges().Empty())
	{
		parameters.pushConstantRangeCount = 0;
		parameters.pushConstantRanges = nullptr;
	}

	else
	{
		pushConstantRanges.Reserve(renderPass->GetPushConstantRanges().Size());

		for (const PushConstantRange &pushConstantRange : renderPass->GetPushConstantRanges())
		{
			pushConstantRanges.EmplaceFast(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
		}

		parameters.pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
		parameters.pushConstantRanges = pushConstantRanges.Data();
	}
	
	parameters.shaderModules.Reserve(5);
	if (renderPass->GetVertexShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetVertexShader())]);
	if (renderPass->GetTessellationControlShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetTessellationControlShader())]);
	if (renderPass->GetTessellationEvaluationShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetTessellationEvaluationShader())]);
	if (renderPass->GetGeometryShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetGeometryShader())]);
	if (renderPass->GetFragmentShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetFragmentShader())]);
	parameters.topology = VulkanTranslationUtilities::GetVulkanTopology(renderPass->GetTopology());

	DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	vertexInputAttributeDescriptions.Reserve(renderPass->GetVertexInputAttributeDescriptions().Size());

	for (const VertexInputAttributeDescription &vertexInputAttributeDescription : renderPass->GetVertexInputAttributeDescriptions())
	{
		vertexInputAttributeDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
	}

	parameters.vertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	parameters.vertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

	DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	vertexInputBindingDescriptions.Reserve(renderPass->GetVertexInputBindingDescriptions().Size());

	for (const VertexInputBindingDescription &vertexInputBindingDescription : renderPass->GetVertexInputBindingDescriptions())
	{
		vertexInputBindingDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
	}

	parameters.vertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
	parameters.vertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
	parameters.viewportExtent = VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height };

	//Create the pipeline!
	pipelines[INDEX(renderPass->GetStage())] = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	vulkanRenderPassData[INDEX(renderPass->GetStage())].framebuffers.Reserve(pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().GetFrameBuffers().Size());

	for (const VulkanFramebuffer& framebuffer : pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().GetFrameBuffers())
	{
		vulkanRenderPassData[INDEX(renderPass->GetStage())].framebuffers.EmplaceFast(framebuffer.Get());
	}

	vulkanRenderPassData[INDEX(renderPass->GetStage())].pipeline = pipelines[INDEX(renderPass->GetStage())]->Get();
	vulkanRenderPassData[INDEX(renderPass->GetStage())].pipelineLayout = pipelines[INDEX(renderPass->GetStage())]->GetPipelineLayout();
	vulkanRenderPassData[INDEX(renderPass->GetStage())].renderPass = pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().Get();

	renderPass->SetData(&vulkanRenderPassData[INDEX(renderPass->GetStage())]);

	//Add the command buffers.
	const uint64 numberOfCommandBuffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	renderPass->SetNumberOfCommandBuffers(numberOfCommandBuffers);

	//Create the directional shadow command pool.
	VulkanCommandPool *const RESTRICT pipelineCommandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) };

	for (uint64 i = 0; i < numberOfCommandBuffers; ++i)
	{
		VulkanCommandBuffer pipelineCommandBuffer;
		pipelineCommandPool->AllocateSecondaryCommandBuffer(pipelineCommandBuffer);
		renderPass->AddCommandBuffer(new VulkanTranslationCommandBuffer(pipelineCommandBuffer));
	}
}

/*
*	Returns the current dynamic uniform data descriptor set.
*/
DescriptorSetHandle VulkanRenderingSystem::GetCurrentDynamicUniformDataDescriptorSet() NOEXCEPT
{
	return frameData.GetCurrentDynamicUniformDataDescriptorSet()->Get();
}

/*
*	Returns the current environment data descriptor set.
*/
DescriptorSetHandle VulkanRenderingSystem::GetCurrentEnvironmentDataDescriptorSet() NOEXCEPT
{
	return frameData.GetCurrentEnvironmentDescriptorSet()->Get();
}

/*
*	Returns the current ocean descriptor set.
*/
DescriptorSetHandle VulkanRenderingSystem::GetCurrentOceanDescriptorSet() NOEXCEPT
{
	return frameData.GetCurrentOceanDescriptorSet()->Get();
}

/*
*	Returns the lighting descriptor set.
*/
DescriptorSetHandle VulkanRenderingSystem::GetLightingDescriptorSet() NOEXCEPT
{
	return descriptorSets[INDEX(DescriptorSet::Lighting)].Get();
}

/*
*	Returns the post processing descriptor set.
*/
DescriptorSetHandle VulkanRenderingSystem::GetPostProcessingDescriptorSet() NOEXCEPT
{
	return descriptorSets[INDEX(DescriptorSet::PostProcessing)].Get();
}

/*
*	Returns the current directional shadow event.
*/
EventHandle VulkanRenderingSystem::GetCurrentDirectionalShadowEvent() NOEXCEPT
{
	return frameData.GetCurrentDirectionalShadowEvent()->Get();
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
	renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
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
	{
		//Initialize the cube map fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetCubeMapFragmentShaderData(data);
		shaderModules[INDEX(Shader::CubeMapFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the cube map vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetCubeMapVertexShaderData(data);
		shaderModules[INDEX(Shader::CubeMapVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadowInstanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowInstancedPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::DirectionalInstancedPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowTerrainTessellationEvaluationShaderData(data);
		shaderModules[INDEX(Shader::DirectionalTerrainShadowTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize the instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetInstancedPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::InstancedPhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the lighting fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLightingFragmentShaderData(data);
		shaderModules[INDEX(Shader::LightingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOceanFragmentShaderData(data);
		shaderModules[INDEX(Shader::OceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the particle system fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemFragmentShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the particle system geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemGeometryShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the particle system vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemVertexShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the post processing fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingFragmentShaderData(data);
		shaderModules[INDEX(Shader::PostProcessingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalFragmentShaderData(data);
		shaderModules[INDEX(Shader::PhysicalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::PhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the shadow map blur fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetShadowMapBlurFragmentShaderData(data);
		shaderModules[INDEX(Shader::ShadowMapBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the shadow map fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetShadowMapFragmentShaderData(data);
		shaderModules[INDEX(Shader::ShadowMapFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain scene buffer fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferFragmentShaderData(data);
		shaderModules[INDEX(Shader::TerrainFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation control shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationControlShaderData(data);
		shaderModules[INDEX(Shader::TerrainTessellationControl)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationEvaluationShaderData(data);
		shaderModules[INDEX(Shader::TerrainTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}
	
	{
		//Initialize the terrain scene buffer vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferVertexShaderData(data);
		shaderModules[INDEX(Shader::TerrainVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationFragmentShaderData(data);
		shaderModules[INDEX(Shader::VegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the vegetation geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationGeometryShaderData(data);
		shaderModules[INDEX(Shader::VegetationGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}
	
	{
		//Initialize the vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationVertexShaderData(data);
		shaderModules[INDEX(Shader::VegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the viewport vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetViewportVertexShaderData(data);
		shaderModules[INDEX(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
}

/*
*	Initializes all pipelines.
*/
void VulkanRenderingSystem::InitializePipelines() NOEXCEPT
{
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
		shadowMapBlurPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(Shader::ViewportVertex)]);
		shadowMapBlurPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(Shader::ShadowMapBlurFragment)]);
		shadowMapBlurPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		shadowMapBlurPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
		shadowMapBlurPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
		shadowMapBlurPipelineCreationParameters.vertexInputBindingDescriptionCount = 0;
		shadowMapBlurPipelineCreationParameters.vertexInputBindingDescriptions = nullptr;
		shadowMapBlurPipelineCreationParameters.viewportExtent = VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION };

		pipelines[INDEX(RenderPassStage::ShadowMapBlur)] = VulkanInterface::Instance->CreatePipeline(shadowMapBlurPipelineCreationParameters);
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
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 1),
			renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetWriteDescriptorSet(descriptorSets[INDEX(DescriptorSet::Lighting)], 2),
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
*	Concatenates all secondary command buffers into the previous one.
*/
void VulkanRenderingSystem::ConcatenateCommandBuffers() NOEXCEPT
{
	//Begin the current directional shadow command buffer.
	VulkanCommandBuffer *const RESTRICT currentDirectionalShadowCommandBuffer{ frameData.GetCurrentDirectionalShadowCommandBuffer() };
	currentDirectionalShadowCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Begin the terrain shadow render pass.
	currentDirectionalShadowCommandBuffer->CommandBeginRenderPassAndClear<2>(pipelines[INDEX(RenderPassStage::DirectionalTerrainShadow)]->GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the terrain shadow.
	currentDirectionalShadowCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(DirectionalTerrainShadowRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the render pass.
	currentDirectionalShadowCommandBuffer->CommandEndRenderPass();

	//Begin the instanced physical shadow render pass.
	currentDirectionalShadowCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::DirectionalInstancedPhysicalShadow)]->GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the instanced physical shadow.
	currentDirectionalShadowCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(DirectionalInstancedPhysicalShadowRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the render pass.
	currentDirectionalShadowCommandBuffer->CommandEndRenderPass();

	//Bind the shadow map blur pipeline.
	currentDirectionalShadowCommandBuffer->CommandBindPipeline(pipelines[INDEX(RenderPassStage::ShadowMapBlur)]->Get());

	//Bind the shadow map blur render pass.
	currentDirectionalShadowCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(RenderPassStage::ShadowMapBlur)]->GetRenderPass(), 0, VkExtent2D{ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SUBPASS_CONTENTS_INLINE);

	//Bind the shadow map blur descriptor set.
	StaticArray<VkDescriptorSet, 2> shadowMapBlurDescriptorSets
	{
		currentDynamicUniformDataDescriptorSet->Get(),
		descriptorSets[INDEX(DescriptorSet::ShadowMapBlur)].Get()
	};

	currentDirectionalShadowCommandBuffer->CommandBindDescriptorSets(pipelines[INDEX(RenderPassStage::ShadowMapBlur)]->GetPipelineLayout(), 0, static_cast<uint32>(shadowMapBlurDescriptorSets.Size()), shadowMapBlurDescriptorSets.Data());

	//Draw the viewport!
	currentDirectionalShadowCommandBuffer->CommandDraw(4, 1);

	//End the shadow map blur render pass.
	currentDirectionalShadowCommandBuffer->CommandEndRenderPass();

	//Signal the directional shadow event.
	currentDirectionalShadowCommandBuffer->CommandSetEvent(frameData.GetCurrentDirectionalShadowEvent()->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	//End the command buffer.
	currentDirectionalShadowCommandBuffer->End();

	//Submit the directional shadow command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(*currentDirectionalShadowCommandBuffer, 0, nullptr, 0, 0, nullptr, nullptr);

	//Begin up the primary command buffer.
	VulkanCommandBuffer *const RESTRICT currentPrimaryCommandBuffer{ frameData.GetCurrentPrimaryCommandBuffer() };
	currentPrimaryCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Begin the terrain render pass.
	currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear<4>(pipelines[INDEX(RenderPassStage::Terrain)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the terrain.
	currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(TerrainRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the terrain render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();

	if (ComponentManager::GetNumberOfStaticPhysicalComponents() > 0)
	{
		//Begin the static physical entities render pass.
		currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::StaticPhysical)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Record the execute command for the static physical entities.
		currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(StaticPhysicalRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

		//End the static physical entities render pass.
		currentPrimaryCommandBuffer->CommandEndRenderPass();
	}

	if (ComponentManager::GetNumberOfInstancedPhysicalComponents() > 0)
	{
		//Begin the instanced physical entities render pass.
		currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::InstancedPhysical)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Record the execute command for the instanced physical entities.
		currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(InstancedPhysicalRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

		//End the instanced physical entities render pass.
		currentPrimaryCommandBuffer->CommandEndRenderPass();
	}

	if (ComponentManager::GetNumberOfVegetationComponents() > 0)
	{
		//Begin the vegetation entities render pass.
		currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::Vegetation)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Record the execute command for the vegetation entities.
		currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(VegetationRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

		//End the vegetation entities render pass.
		currentPrimaryCommandBuffer->CommandEndRenderPass();
	}

	//Begin the lighting render pass.
	currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear<2>(pipelines[INDEX(RenderPassStage::Lighting)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the lighting.
	currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(LightingRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the lighting render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();

	//Reset the directional shadow event.
	currentPrimaryCommandBuffer->CommandResetEvent(frameData.GetCurrentDirectionalShadowEvent()->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	//Begin the sky render pass.
	currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::Sky)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the sky.
	currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(SkyRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the sky render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();

	if (ComponentManager::GetNumberOfParticleSystemComponents() > 0)
	{
		//Begin the particle system entities render pass.
		currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(RenderPassStage::ParticleSystem)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		//Record the execute command for the particle system entities.
		currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(ParticleSystemRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

		//End the static particle system render pass.
		currentPrimaryCommandBuffer->CommandEndRenderPass();
	}

	//Bind the ocean render pass.
	currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(RenderPassStage::Ocean)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the ocean.
	currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(OceanRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the static particle system render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();

	//Bind the post processing render pass.
	currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(RenderPassStage::PostProcessing)]->GetRenderPass(), frameData.GetCurrentFrame(), VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Record the execute command for the post processing.
	currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<VulkanTranslationCommandBuffer *const RESTRICT>(PostProcessingRenderPass::Instance->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());

	//End the post processing render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();
}

/*
*	Ends the frame.
*/
void VulkanRenderingSystem::EndFrame() NOEXCEPT
{
	//End the current command buffer.
	frameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(*frameData.GetCurrentPrimaryCommandBuffer(), 1, semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, semaphores[INDEX(GraphicsSemaphore::RenderFinished)], frameData.GetCurrentFence()->Get());
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
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(oceanDescriptorSet, 1),
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