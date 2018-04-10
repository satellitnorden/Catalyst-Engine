//Header file.
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/InstancedPhysicalEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/SpotLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>
#include <Entities/WaterEntity.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Matrix3.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/PhysicalMaterial.h>
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/RenderingUtilities.h>
#include <Rendering/Engine Layer/ShaderLoader.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/WaterMaterial.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationUtilities.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/TaskSystem.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

//System definition.
DEFINE_SYSTEM(VulkanRenderingSystem);

/*
*	Default constructor.
*/
VulkanRenderingSystem::VulkanRenderingSystem() NOEXCEPT
{

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
	frameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(), descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)]);
}

/*
*	Updates the graphics system synchronously.
*/
void VulkanRenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Execute the update view frustum culling task.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateViewFrustumCulling();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateViewFrustumCuling)]));

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

	//Render the terrain.
	RenderTerrain();

	//Render all static physical entities.
	RenderStaticPhysicalEntities();

	//Render all instanced physical entities.
	RenderInstancedPhysicalEntities();

	//Render all vegetation entities.
	RenderVegetationEntities();

	//Render the lighting.
	RenderLighting();

	//Render the sky box.
	RenderSkyBox();

	//Render water.
	RenderWater();

	//Render the post processing.
	RenderPostProcessing();

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
*	Creates and returns physical model.
*/
void VulkanRenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ physicalModelData.vertices.Data(), physicalModelData.indices.Data() };
	const VkDeviceSize modelDataSizes[]{ sizeof(PhysicalVertex) * physicalModelData.vertices.Size(), sizeof(uint32) * physicalModelData.indices.Size() };
	GraphicsBufferHandle buffer = VulkanInterface::Instance->CreateBuffer(modelData, modelDataSizes, 2);

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
void VulkanRenderingSystem::CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT
{
	//Load the normal map.
	waterMaterial.normalMapTexture = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(waterMaterialData.width, waterMaterialData.height, 4, waterMaterialData.normalMapData, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT));
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
	VulkanBuffer *RESTRICT terrainVertexBuffer{ VulkanInterface::Instance->CreateBuffer(terrainData, terrainDataSizes, 2) };

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
	renderComponent.buffer = *static_cast<VulkanBuffer *RESTRICT>(model.GetBuffer());
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
	VulkanBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateBuffer(transformationsData, transformationsDataSizes, 1);

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
void VulkanRenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const DynamicArray<VegetationTransformation> &transformations) const NOEXCEPT
{
	//Create the positions buffer.
	const void *RESTRICT transformationsData[]{ transformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(VegetationTransformation) * transformations.Size() };
	VulkanBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the vegetation entity component with the relevant data.
	VegetationComponent &component{ ComponentManager::GetVegetationComponents()[entity.GetComponentsIndex()] };

	component.transformationsBuffer = transformationsBuffer;
	component.instanceCount = static_cast<uint32>(transformations.Size());
}

/*
*	Initializes a water entity.
*/
void VulkanRenderingSystem::InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT
{
	//Fill the terrain entity components with the relevant data.
	WaterComponent &waterComponent{ ComponentManager::GetWaterComponents()[waterEntity->GetComponentsIndex()] };
	WaterRenderComponent &waterRenderComponent{ ComponentManager::GetWaterRenderComponents()[waterEntity->GetComponentsIndex()] };

	waterComponent.waterUniformData = waterUniformData;
	waterComponent.uniformBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(WaterUniformData));
	static_cast<const VulkanUniformBuffer *RESTRICT>(waterComponent.uniformBuffer)->UploadData(&waterComponent.waterUniformData);

	//Create the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(waterRenderComponent.descriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::Water)]);

	StaticArray<VkWriteDescriptorSet, 4> writeDescriptorSets
	{
		static_cast<const VulkanUniformBuffer *RESTRICT>(waterComponent.uniformBuffer)->GetWriteDescriptorSet(waterRenderComponent.descriptorSet, 1),
		static_cast<const VulkanRenderTarget *RESTRICT>(renderTargets[INDEX(RenderTarget::WaterScene)])->GetWriteDescriptorSet(waterRenderComponent.descriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(waterMaterial.normalMapTexture)->GetWriteDescriptorSet(waterRenderComponent.descriptorSet, 3),
		static_cast<const VulkanCubeMapTexture *const RESTRICT>(environmentMaterial.diffuseTexture)->GetWriteDescriptorSet(waterRenderComponent.descriptorSet, 4)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
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
*	Sets the environment material.
*/
void VulkanRenderingSystem::SetEnvironmentMaterial(const EnvironmentMaterial &newEnvioronmentMaterial) NOEXCEPT
{
	//Update the environment material.
	environmentMaterial = newEnvioronmentMaterial;

	//Allocate the sky box descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(skyBoxDescriptorSet, descriptorSetLayouts[INDEX(DescriptorSetLayout::CubeMap)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
	{
		static_cast<const VulkanCubeMapTexture *const RESTRICT>(environmentMaterial.diffuseTexture)->GetWriteDescriptorSet(skyBoxDescriptorSet, 1)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Reinitialize the descriptor sets.
	ReinitializeDescriptorSets();
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
	depthBuffers[DepthBuffer::SceneBufferDepthBuffer] = VulkanInterface::Instance->CreateDepthBuffer(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Initialize all render targets.
	renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[INDEX(RenderTarget::Scene)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[INDEX(RenderTarget::WaterScene)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
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
	//Initialize the dynamic uniform data descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> dynamicUniformDataDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)].Initialize(1, dynamicUniformDataDescriptorSetLayoutBindings.Data());

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

	//Initialize the scene buffer descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> staticPhysicalDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::Physical)].Initialize(3, staticPhysicalDescriptorSetLayoutBindings.Data());

	//Initialize the lighting descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 6> lightingDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)].Initialize(6, lightingDescriptorSetLayoutBindings.Data());

	//Initialize the cube map descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> cubeMapDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::CubeMap)].Initialize(1, cubeMapDescriptorSetLayoutBindings.Data());

	//Initialize the water descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> waterDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::Water)].Initialize(4, waterDescriptorSetLayoutBindings.Data());

	//Initialize the post processing descriptor set layout.
	constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> postProcessingDescriptorSetLayoutBindings
	{
		VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
		VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)].Initialize(2, postProcessingDescriptorSetLayoutBindings.Data());
}

/*
*	Initializes all shader modules.
*/
void VulkanRenderingSystem::InitializeShaderModules() NOEXCEPT
{
	//Initialize the cube map fragment shader module.
	const DynamicString &shadersPath{ EngineSystem::Instance->GetProjectInformation().renderingInformation.shadersPath };

	const auto cubeMapFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "CubeMapFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::CubeMapFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(cubeMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the cube map vertex shader module.
	const auto cubeMapVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "CubeMapVertexShader.spv");
	shaderModules[INDEX(ShaderModule::CubeMapVertexShader)] = VulkanInterface::Instance->CreateShaderModule(cubeMapVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the instanced physical vertex shader module.
	const auto instancedPhysicalVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "InstancedPhysicalVertexShader.spv");
	shaderModules[INDEX(ShaderModule::InstancedPhysicalVertexShader)] = VulkanInterface::Instance->CreateShaderModule(instancedPhysicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting fragment shader module.
	const auto lightingFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "LightingFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::LightingFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(lightingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the post processing fragment shader module.
	const auto postProcessingFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PostProcessingFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::PostProcessingFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(postProcessingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the physical fragment shader module.
	const auto physicalFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PhysicalFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::PhysicalFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(physicalFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the physical vertex shader module.
	const auto physicalVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "PhysicalVertexShader.spv");
	shaderModules[INDEX(ShaderModule::PhysicalVertexShader)] = VulkanInterface::Instance->CreateShaderModule(physicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the terrain scene buffer fragment shader module.
	const auto terrainSceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::TerrainFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the terrain scene buffer tessellation control shader module.
	const auto terrainSceneBufferTessellationControlShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferTessellationControlShader.spv");
	shaderModules[INDEX(ShaderModule::TerrainTessellationControlShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationControlShaderByteCode, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);

	//Initialize the terrain scene buffer tessellation evaluation shader module.
	const auto terrainSceneBufferTessellationEvaluationShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferTessellationEvaluationShader.spv");
	shaderModules[INDEX(ShaderModule::TerrainTessellationEvaluationShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationEvaluationShaderByteCode, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

	//Initialize the terrain scene buffer vertex shader module.
	const auto terrainSceneBufferVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "TerrainSceneBufferVertexShader.spv");
	shaderModules[INDEX(ShaderModule::TerrainVertexShader)] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the vegetation fragment shader module.
	const auto vegetationFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::VegetationFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the vegetation geometry shader module.
	const auto vegetationGeometryShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationGeometryShader.spv");
	shaderModules[INDEX(ShaderModule::VegetationGeometryShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationGeometryShaderByteCode, VK_SHADER_STAGE_GEOMETRY_BIT);

	//Initialize the vegetation vertex shader module.
	const auto vegetationVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "VegetationVertexShader.spv");
	shaderModules[INDEX(ShaderModule::VegetationVertexShader)] = VulkanInterface::Instance->CreateShaderModule(vegetationVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the viewport vertex shader module.
	const auto viewportVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "ViewportVertexShader.spv");
	shaderModules[INDEX(ShaderModule::ViewportVertexShader)] = VulkanInterface::Instance->CreateShaderModule(viewportVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the water fragment shader module.
	const auto waterFragmentShaderByteCode = ShaderLoader::LoadShader(shadersPath + "WaterFragmentShader.spv");
	shaderModules[INDEX(ShaderModule::WaterFragmentShader)] = VulkanInterface::Instance->CreateShaderModule(waterFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the water vertex shader module.
	const auto waterVertexShaderByteCode = ShaderLoader::LoadShader(shadersPath + "WaterVertexShader.spv");
	shaderModules[INDEX(ShaderModule::WaterVertexShader)] = VulkanInterface::Instance->CreateShaderModule(waterVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
}

/*
*	Initializes all pipelines.
*/
void VulkanRenderingSystem::InitializePipelines() NOEXCEPT
{
	//Create the terrain pipeline.
	VulkanPipelineCreationParameters terrainPipelineCreationParameters;

	terrainPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
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
	terrainPipelineCreationParameters.depthBuffers.Reserve(1);
	terrainPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
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

	//Create the static physical pipeline.
	VulkanPipelineCreationParameters staticPhysicalPipelineCreationParameters;

	staticPhysicalPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
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
	staticPhysicalPipelineCreationParameters.depthBuffers.Reserve(1);
	staticPhysicalPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
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

	{
		//Create the instanced physical pipeline.
		VulkanPipelineCreationParameters instancedPhysicalPipelineCreationParameters;

		instancedPhysicalPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
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
		instancedPhysicalPipelineCreationParameters.depthBuffers.Reserve(1);
		instancedPhysicalPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
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
		//Create the instanced physical pipeline.
		VulkanPipelineCreationParameters vegetationPipelineCreationParameters;

		vegetationPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
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
		vegetationPipelineCreationParameters.depthBuffers.Reserve(1);
		vegetationPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
		vegetationPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
		vegetationPipelineCreationParameters.depthTestEnable = VK_TRUE;
		vegetationPipelineCreationParameters.depthWriteEnable = VK_TRUE;
		StaticArray<VulkanDescriptorSetLayout, 1> vegetationDescriptorSetLayouts
		{
			descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)]
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
		StaticArray<VkVertexInputAttributeDescription, 4> vegetationVertexInputAttributeDescriptions;
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

	//Create the physical pipeline.
	VulkanPipelineCreationParameters lightingPipelineCreationParameters;

	lightingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
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
	StaticArray<VulkanDescriptorSetLayout, 2> lightingDescriptorSetLayouts
	{
		descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
		descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)]
	};
	lightingPipelineCreationParameters.descriptorSetLayoutCount = 2;
	lightingPipelineCreationParameters.descriptorSetLayouts = lightingDescriptorSetLayouts.Data();
	lightingPipelineCreationParameters.pushConstantRangeCount = 0;
	lightingPipelineCreationParameters.pushConstantRanges = nullptr;
	lightingPipelineCreationParameters.shaderModules.Reserve(2);
	lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
	lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::LightingFragmentShader)]);
	lightingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	lightingPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
	lightingPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
	VkVertexInputBindingDescription lightingVertexInputBindingDescription;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(lightingVertexInputBindingDescription);
	lightingPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
	lightingPipelineCreationParameters.vertexInputBindingDescriptions = &lightingVertexInputBindingDescription;
	lightingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[INDEX(Pipeline::Lighting)] = VulkanInterface::Instance->CreatePipeline(lightingPipelineCreationParameters);

	//Create the cube map pipeline.
	VulkanPipelineCreationParameters cubeMapPipelineCreationParameters;

	cubeMapPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
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
	cubeMapPipelineCreationParameters.depthBuffers.Reserve(1);
	cubeMapPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	cubeMapPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	cubeMapPipelineCreationParameters.depthTestEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	StaticArray<VulkanDescriptorSetLayout, 2> cubeMapDescriptorSetLayouts
	{
		descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
		descriptorSetLayouts[INDEX(DescriptorSetLayout::CubeMap)]
	};
	cubeMapPipelineCreationParameters.descriptorSetLayoutCount = 2;
	cubeMapPipelineCreationParameters.descriptorSetLayouts = cubeMapDescriptorSetLayouts.Data();
	cubeMapPipelineCreationParameters.pushConstantRangeCount = 0;
	cubeMapPipelineCreationParameters.pushConstantRanges = nullptr;
	cubeMapPipelineCreationParameters.shaderModules.Reserve(2);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::CubeMapVertexShader)]);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::CubeMapFragmentShader)]);
	cubeMapPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	cubeMapPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
	cubeMapPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
	VkVertexInputBindingDescription cubeMapVertexInputBindingDescription;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(cubeMapVertexInputBindingDescription);
	cubeMapPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
	cubeMapPipelineCreationParameters.vertexInputBindingDescriptions = &cubeMapVertexInputBindingDescription;
	cubeMapPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[INDEX(Pipeline::CubeMap)] = VulkanInterface::Instance->CreatePipeline(cubeMapPipelineCreationParameters);

	//Create the water pipeline.
	VulkanPipelineCreationParameters waterPipelineCreationParameters;

	waterPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	waterPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	waterPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	waterPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	waterPipelineCreationParameters.colorAttachments.UpsizeSlow(1);
	waterPipelineCreationParameters.colorAttachments[0].Reserve(1);
	waterPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(RenderTarget::Scene)]->GetImageView());
	waterPipelineCreationParameters.cullMode = VK_CULL_MODE_NONE;
	waterPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	waterPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	waterPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	waterPipelineCreationParameters.depthBuffers.Reserve(1);
	waterPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	waterPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	waterPipelineCreationParameters.depthTestEnable = VK_TRUE;
	waterPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	StaticArray<VulkanDescriptorSetLayout, 2> waterDescriptorSetLayouts
	{
		descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
		descriptorSetLayouts[INDEX(DescriptorSetLayout::Water)]
	};
	waterPipelineCreationParameters.descriptorSetLayoutCount = 2;
	waterPipelineCreationParameters.descriptorSetLayouts = waterDescriptorSetLayouts.Data();
	waterPipelineCreationParameters.pushConstantRangeCount = 0;
	waterPipelineCreationParameters.pushConstantRanges = nullptr;
	waterPipelineCreationParameters.shaderModules.Reserve(2);
	waterPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::WaterVertexShader)]);
	waterPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::WaterFragmentShader)]);
	waterPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	StaticArray<VkVertexInputAttributeDescription, 2> waterVertexInputAttributeDescriptions;
	VulkanTranslationUtilities::GetTerrainVertexInputAttributeDescriptions(waterVertexInputAttributeDescriptions);
	waterPipelineCreationParameters.vertexInputAttributeDescriptionCount = 2;
	waterPipelineCreationParameters.vertexInputAttributeDescriptions = waterVertexInputAttributeDescriptions.Data();
	VkVertexInputBindingDescription waterVertexInputBindingDescription;
	VulkanTranslationUtilities::GetTerrainVertexInputBindingDescription(waterVertexInputBindingDescription);
	terrainPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
	terrainPipelineCreationParameters.vertexInputBindingDescriptions = &waterVertexInputBindingDescription;
	waterPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[INDEX(Pipeline::Water)] = VulkanInterface::Instance->CreatePipeline(waterPipelineCreationParameters);

	//Create the post processing pipeline.
	VulkanPipelineCreationParameters postProcessingPipelineCreationParameters;

	postProcessingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
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
	postProcessingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	postProcessingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	postProcessingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	StaticArray<VulkanDescriptorSetLayout, 2> postProcessingDescriptorSetLayouts
	{
		descriptorSetLayouts[INDEX(DescriptorSetLayout::DynamicUniformData)],
		descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)]
	};
	postProcessingPipelineCreationParameters.descriptorSetLayoutCount = 2;
	postProcessingPipelineCreationParameters.descriptorSetLayouts = postProcessingDescriptorSetLayouts.Data();
	postProcessingPipelineCreationParameters.pushConstantRangeCount = 0;
	postProcessingPipelineCreationParameters.pushConstantRanges = nullptr;
	postProcessingPipelineCreationParameters.shaderModules.Reserve(2);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::ViewportVertexShader)]);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[INDEX(ShaderModule::PostProcessingFragmentShader)]);
	postProcessingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	postProcessingPipelineCreationParameters.vertexInputAttributeDescriptionCount = 0;
	postProcessingPipelineCreationParameters.vertexInputAttributeDescriptions = nullptr;
	VkVertexInputBindingDescription postProcessingInputBindingDescription;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(postProcessingInputBindingDescription);
	postProcessingPipelineCreationParameters.vertexInputBindingDescriptionCount = 1;
	postProcessingPipelineCreationParameters.vertexInputBindingDescriptions = &postProcessingInputBindingDescription;
	postProcessingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[INDEX(Pipeline::PostProcessing)] = VulkanInterface::Instance->CreatePipeline(postProcessingPipelineCreationParameters);
}

/*
*	Initializes all descriptor sets.
*/
void VulkanRenderingSystem::InitializeDescriptorSets() NOEXCEPT
{
	{
		//Initialize the lighting descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], descriptorSetLayouts[INDEX(DescriptorSetLayout::Lighting)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 4> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1),
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3),
			renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], descriptorSetLayouts[INDEX(DescriptorSetLayout::PostProcessing)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 2> writeDescriptorSets
		{
			uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 1),
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 2)
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
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Set the current frame.
	frameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Set the current command buffer.
	currentCommandBuffer = frameData.GetCurrentCommandBuffer();

	//Set the current dynamic uniform data descriptor set.
	currentDynamicUniformDataDescriptorSet = frameData.GetCurrentDynamicUniformDataDescriptorSet();

	//Wait for the current fence to finish.
	frameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	frameData.GetCurrentFence()->Reset();

	//Execute the update dynamic uniform data task.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateDynamicUniformData();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateDynamicUniformData)]));

	//Set up the current command buffer.
	currentCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

/*
*	Renders the terrain.
*/
void VulkanRenderingSystem::RenderTerrain() NOEXCEPT
{
	//Cache the pipeline.
	VulkanPipeline &terrainSceneBufferPipeline{ *pipelines[INDEX(Pipeline::Terrain)] };

	//Begin the pipeline and render pass.
	currentCommandBuffer->CommandBindPipeline(terrainSceneBufferPipeline);
	currentCommandBuffer->CommandBeginRenderPassAndClear<4>(terrainSceneBufferPipeline.GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

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

		currentCommandBuffer->CommandBindDescriptorSets(terrainSceneBufferPipeline, 2, terrainDescriptorSets.Data());
		currentCommandBuffer->CommandBindVertexBuffers(1, &static_cast<const VulkanBuffer *RESTRICT>(terrainRenderComponent->vertexAndIndexBuffer)->Get(), &offset);
		currentCommandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanBuffer *RESTRICT>(terrainRenderComponent->vertexAndIndexBuffer), terrainRenderComponent->indexBufferOffset);
		currentCommandBuffer->CommandDrawIndexed(terrainRenderComponent->indexCount, 1);
	}

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders all static physical entities.
*/
void VulkanRenderingSystem::RenderStaticPhysicalEntities() NOEXCEPT
{
	//Wait for the update view frustum culling  task to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateViewFrustumCuling)].WaitFor();

	//Cache the pipeline.
	VulkanPipeline &StaticPhysicalPipeline{ *pipelines[INDEX(Pipeline::StaticPhysical)] };

	//Iterate over all static physical entity components and draw them all.
	const uint64 numberOfStaticPhysicalEntityComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };

	if (numberOfStaticPhysicalEntityComponents == 0)
	{
		return;
	}

	const FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents() };
	const StaticPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents() };

	//Begin the pipeline and render pass.
	currentCommandBuffer->CommandBindPipeline(StaticPhysicalPipeline);
	currentCommandBuffer->CommandBeginRenderPass(StaticPhysicalPipeline.GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	for (uint64 i = 0; i < numberOfStaticPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++renderComponent)
	{
		//Don't draw this static physical entity if it isn't in the view frustum.
		if (!frustumCullingComponent->isInViewFrustum)
			continue;

		StaticArray<VulkanDescriptorSet, 2> staticPhysicalEntityDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			renderComponent->descriptorSet
		};

		const VkDeviceSize offset{ 0 };

		currentCommandBuffer->CommandPushConstants(StaticPhysicalPipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4), &renderComponent->modelMatrix);
		currentCommandBuffer->CommandBindDescriptorSets(StaticPhysicalPipeline, 2, staticPhysicalEntityDescriptorSets.Data());
		currentCommandBuffer->CommandBindVertexBuffers(1, &renderComponent->buffer.Get(), &offset);
		currentCommandBuffer->CommandBindIndexBuffer(renderComponent->buffer, renderComponent->indexOffset);
		currentCommandBuffer->CommandDrawIndexed(renderComponent->indexCount, 1);
	}

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders all instanced physical entities.
*/
void VulkanRenderingSystem::RenderInstancedPhysicalEntities() NOEXCEPT
{
	//Cache the pipeline.
	VulkanPipeline &instancedPhysicalPipeline{ *pipelines[INDEX(Pipeline::InstancedPhysical)] };

	//Iterate over all instanced physical entity components and draw them all.
	const uint64 numberOfInstancedPhysicalComponents{ ComponentManager::GetNumberOfInstancedPhysicalComponents() };

	//If there's none to draw - draw none!
	if (numberOfInstancedPhysicalComponents == 0)
	{
		return;
	}

	const InstancedPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetInstancedPhysicalRenderComponents() };

	//Begin the pipeline and render pass.
	currentCommandBuffer->CommandBindPipeline(instancedPhysicalPipeline);
	currentCommandBuffer->CommandBeginRenderPass(instancedPhysicalPipeline.GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	for (uint64 i = 0; i < numberOfInstancedPhysicalComponents; ++i, ++renderComponent)
	{
		StaticArray<VulkanDescriptorSet, 2> instancedPhysicalDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			renderComponent->descriptorSet
		};

		StaticArray<VkBuffer, 2> instancedPhysicalBuffers
		{
			static_cast<const VulkanBuffer *RESTRICT>(renderComponent->modelBuffer)->Get(),
			static_cast<const VulkanBuffer *RESTRICT>(renderComponent->transformationsBuffer)->Get()
		};

		StaticArray<VkDeviceSize, 2> offsets
		{
			static_cast<VkDeviceSize>(0),
			static_cast<VkDeviceSize>(0)
		};

		currentCommandBuffer->CommandBindDescriptorSets(instancedPhysicalPipeline, 2, instancedPhysicalDescriptorSets.Data());
		currentCommandBuffer->CommandBindVertexBuffers(2, instancedPhysicalBuffers.Data(), offsets.Data());
		currentCommandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanBuffer *RESTRICT>(renderComponent->modelBuffer), renderComponent->indexOffset);
		currentCommandBuffer->CommandDrawIndexed(renderComponent->indexCount, renderComponent->instanceCount);
	}

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders all vegetation entities.
*/
void VulkanRenderingSystem::RenderVegetationEntities() NOEXCEPT
{
	//Cache the pipeline.
	VulkanPipeline &vegetationPipeline{ *pipelines[INDEX(Pipeline::Vegetation)] };

	//Iterate over all vegetation entity components and draw them all.
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there's none to draw - draw none!
	if (numberOfVegetationComponents == 0)
	{
		return;
	}

	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationComponents() };

	//Begin the pipeline and render pass.
	currentCommandBuffer->CommandBindPipeline(vegetationPipeline);
	currentCommandBuffer->CommandBeginRenderPass(vegetationPipeline.GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		constexpr VkDeviceSize offset{ 0 };

		currentCommandBuffer->CommandBindDescriptorSets(vegetationPipeline, 1, currentDynamicUniformDataDescriptorSet);
		currentCommandBuffer->CommandBindVertexBuffers(1, &static_cast<const VulkanBuffer *RESTRICT>(component->transformationsBuffer)->Get(), &offset);
		currentCommandBuffer->CommandDraw(1, component->instanceCount);
	}

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders lighting.
*/
void VulkanRenderingSystem::RenderLighting() NOEXCEPT
{
	//Bind the lighting pipeline.
	currentCommandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::Lighting)]);

	//Bind the lighting render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::Lighting)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Bind the scene buffer descriptor set.
	StaticArray<VulkanDescriptorSet, 2> lightingDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		descriptorSets[DescriptorSet::LightingDescriptorSet]
	};

	currentCommandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::Lighting)], 2, lightingDescriptorSets.Data());

	//Draw the viewport!
	currentCommandBuffer->CommandDraw(4, 1);

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders water.
*/
void VulkanRenderingSystem::RenderWater() NOEXCEPT
{
	//Cache the pipeline.
	VulkanPipeline *RESTRICT waterPipeline{ pipelines[INDEX(Pipeline::Water)] };

	//Begin the pipeline and render pass.
	currentCommandBuffer->CommandBindPipeline(*waterPipeline);
	currentCommandBuffer->CommandBeginRenderPass(waterPipeline->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Iterate over all water entity components and draw them all.
	const uint64 numberOfWaterComponents{ ComponentManager::GetNumberOfWaterComponents() };
	const WaterRenderComponent *RESTRICT waterRenderComponent{ ComponentManager::GetWaterRenderComponents() };

	for (uint64 i = 0; i < numberOfWaterComponents; ++i, ++waterRenderComponent)
	{
		StaticArray<VulkanDescriptorSet, 2> waterDescriptorSets
		{
			*currentDynamicUniformDataDescriptorSet,
			waterRenderComponent->descriptorSet
		};

		currentCommandBuffer->CommandBindDescriptorSets(*waterPipeline, 2, waterDescriptorSets.Data());
		currentCommandBuffer->CommandDraw(4, 1);
	}

	//End the render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders sky box.
*/
void VulkanRenderingSystem::RenderSkyBox() NOEXCEPT
{
	//Bind the cube map pipeline.
	currentCommandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::CubeMap)]);

	//Bind the cube map render pass.
	currentCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(Pipeline::CubeMap)]->GetRenderPass(), 0, VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Bind the sky box descriptor set.
	StaticArray<VulkanDescriptorSet, 2> skyBoxDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		skyBoxDescriptorSet
	};

	currentCommandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::CubeMap)], 2, skyBoxDescriptorSets.Data());

	//Draw the sky box!
	currentCommandBuffer->CommandDraw(36, 1);

	//End the cube map render pass.
	currentCommandBuffer->CommandEndRenderPass();
}

/*
*	Renders the post processing.
*/
void VulkanRenderingSystem::RenderPostProcessing() NOEXCEPT
{
	//Bind the post processing pipeline.
	currentCommandBuffer->CommandBindPipeline(*pipelines[INDEX(Pipeline::PostProcessing)]);

	//Bind the post processing render pass.
	currentCommandBuffer->CommandBeginRenderPassAndClear<1>(pipelines[INDEX(Pipeline::PostProcessing)]->GetRenderPass(), frameData.GetCurrentFrame(), VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Bind the post processing descriptor set.
	StaticArray<VulkanDescriptorSet, 2> postProcessingDescriptorSets
	{
		*currentDynamicUniformDataDescriptorSet,
		descriptorSets[DescriptorSet::PostProcessingDescriptorSet]
	};

	currentCommandBuffer->CommandBindDescriptorSets(*pipelines[INDEX(Pipeline::PostProcessing)], 2, postProcessingDescriptorSets.Data());

	//Draw the viewport!
	currentCommandBuffer->CommandDraw(4, 1);

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

	//Wait for the update dynamic uniform data task to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateDynamicUniformData)].WaitFor();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(*currentCommandBuffer, 1, semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, semaphores[INDEX(GraphicsSemaphore::RenderFinished)], frameData.GetCurrentFence()->Get());
}

/*
*	Re-initializes all descriptor sets.
*/
void VulkanRenderingSystem::ReinitializeDescriptorSets() NOEXCEPT
{
	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 5> writeDescriptorSets
	{
		static_cast<const VulkanCubeMapTexture *const RESTRICT>(environmentMaterial.diffuseIrradianceTexture)->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1),
		static_cast<const VulkanCubeMapTexture *const RESTRICT>(environmentMaterial.diffuseTexture)->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2),
		renderTargets[INDEX(RenderTarget::SceneBufferAlbedoColor)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3),
		renderTargets[INDEX(RenderTarget::SceneBufferNormalDirectionDepth)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4),
		renderTargets[INDEX(RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusion)]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 5)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
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
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightColor = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightScreenSpacePosition = Vector3(0.0f, 0.0f, 0.0f);
	}

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

	//Upload the dynamic uniform data to the uniform buffer.
	frameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&dynamicUniformData));
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

//Preprocessor undefines.
#undef VULKAN_SHADERS_PATH