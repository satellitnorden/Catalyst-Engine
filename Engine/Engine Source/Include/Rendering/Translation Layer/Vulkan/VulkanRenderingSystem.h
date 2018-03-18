#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>


//Math.
#include <Math/Matrix4.h>

//Multithreading.
#include <Multithreading/MultithreadedResource.h>
#include <Multithreading/Semaphore.h>

//Rendering.
#include <Rendering/Engine Layer/PostProcessingUniformData.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/Window.h>
#include <Rendering/Translation Layer/Vulkan/VulkanDynamicUniformData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanFrameData.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

//Forward declarations.
class CameraEntity;
class CPUTexture4;
class InstancedPhysicalEntity;
class PhysicalMaterial;
class PhysicalMaterialData;
class PhysicalModel;
class PhysicalModelData;
class StaticPhysicalEntity;
class TerrainEntity;
class TerrainMaterial;
class TerrainMaterialData;
class TerrainUniformData;
class TextureData;
class WaterEntity;
class WaterMaterialData;
class WaterMaterial;
class WaterUniformData;

class VulkanRenderingSystem final
{

	//System declaration.
	DECLARE_SYSTEM(VulkanRenderingSystem);

public:

	/*
	*	Default constructor.
	*/
	VulkanRenderingSystem() NOEXCEPT;

	/*
	*	Initializes the Vulkan rendering system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the rendering Vulkan system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the Vulkan rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Creates a terrain material.
	*/
	void CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT;

	/*
	*	Creates and returns physical model.
	*/
	void CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT;

	/*
	*	Creates a physical material.
	*/
	void CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT;

	/*
	*	Creates a water material.
	*/
	void CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT;

	/*
	*	Initializes a static physical entity.
	*/
	void InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Initializes a water entity.
	*/
	void InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	TextureCubeMapHandle CreateCubeMapTexture(const char *RESTRICT frontTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT leftTexturePath) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT;

	/*
	*	Sets the active sky box cube map texture.
	*/
	void SetActiveSkyBox(TextureCubeMapHandle newSkyBox) NOEXCEPT;

	/*
	*	Sets the post processing blur amount.
	*/
	void SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT;

	/*
	*	Sets the post processing chromatic aberration amount.
	*/
	void SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT;

	/*
	*	Sets the post processing saturation.
	*/
	void SetPostProcessingSaturation(const float newSaturation) NOEXCEPT;

	/*
	*	Sets the post processing sharpen amount.
	*/
	void SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT;

private:

	//Enumeration covering all default textures.
	enum DefaultTexture : uint8
	{
		Black,
		White,
		NumberOfDefaultTextures
	};

	//Enumeration covering all depth buffers.
	enum DepthBuffer : uint8
	{
		SceneBufferDepthBuffer,
		NumberOfDepthBuffers
	};

	//Enumeration covering all descriptor sets.
	enum DescriptorSet : uint8
	{
		LightingDescriptorSet,
		PostProcessingDescriptorSet,
		NumberOfDescriptorSet
	};

	//Enumeration covering all descriptor set layouts.
	enum class DescriptorSetLayout : uint8
	{
		DynamicUniformData,
		Terrain,
		Physical,
		Lighting,
		Water,
		CubeMap,
		PostProcessing,
		NumberOfDescriptorSetLayouts
	};

	//Enumeration covering all pipelines.
	enum class Pipeline : uint8
	{
		Terrain,
		StaticPhysical,
		InstancedPhysical,
		Lighting,
		CubeMap,
		Water,
		PostProcessing,
		NumberOfPipelines
	};

	//Enumeration covering all render targets.
	enum class RenderTarget : uint8
	{
		SceneBufferAlbedoColor,
		SceneBufferNormalDirectionDepth,
		SceneBufferRoughnessMetallicAmbientOcclusion,
		Scene,
		WaterScene,
		NumberOfRenderTargets
	};

	//Enumeration covering all semaphores.
	enum class GraphicsSemaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	//Enumeration covering all shader modules.
	enum class ShaderModule : uint8
	{
		CubeMapFragmentShader,
		CubeMapVertexShader,
		InstancedPhysicalVertexShader,
		LightingFragmentShader,
		PostProcessingFragmentShader,
		PhysicalFragmentShader,
		PhysicalVertexShader,
		TerrainFragmentShader,
		TerrainTessellationControlShader,
		TerrainTessellationEvaluationShader,
		TerrainVertexShader,
		ViewportVertexShader,
		WaterFragmentShader,
		WaterVertexShader,
		NumberOfShaderModules
	};

	//Enumeration covering all task semaphores.
	enum class TaskSemaphore : uint8
	{
		UpdateDynamicUniformData,
		UpdateViewFrustumCuling,
		NumberOfTaskSemaphores
	};

	//Enumeration covering all uniform buffers.
	enum UniformBuffer : uint8
	{
		PostProcessingUniformDataBuffer,
		NumberOfUniformBuffers
	};

	//Container for all task semaphores.
	StaticArray<Semaphore, INDEX(TaskSemaphore::NumberOfTaskSemaphores)> taskSemaphores;

	//The main window.
	Window mainWindow;

	//The active camera.
	CameraEntity *RESTRICT activeCamera;

	//The current projection matrix.
	Matrix4 projectionMatrix;

	//The dynamic uniform data.
	VulkanDynamicUniformData dynamicUniformData;

	//The post processing uniform data.
	PostProcessingUniformData postProcessingUniformData;

	//Container for all default textures.
	StaticArray<Vulkan2DTexture *RESTRICT, DefaultTexture::NumberOfDefaultTextures> defaultTextures;

	//Container for all depth buffers.
	StaticArray<VulkanDepthBuffer *RESTRICT, DepthBuffer::NumberOfDepthBuffers> depthBuffers;

	//Container for all descriptor sets.
	StaticArray<VulkanDescriptorSet, DescriptorSet::NumberOfDescriptorSet> descriptorSets;

	//Container for all descriptor set layouts.
	StaticArray<VulkanDescriptorSetLayout, INDEX(DescriptorSetLayout::NumberOfDescriptorSetLayouts)> descriptorSetLayouts;

	//Container for all render targets.
	StaticArray<VulkanRenderTarget *RESTRICT, INDEX(RenderTarget::NumberOfRenderTargets)> renderTargets;

	//Container for all pipelines.
	StaticArray<VulkanPipeline *RESTRICT, INDEX(Pipeline::NumberOfPipelines)> pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, INDEX(GraphicsSemaphore::NumberOfSemaphores)> semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, INDEX(ShaderModule::NumberOfShaderModules)> shaderModules;

	//Container for all uniform buffers.
	StaticArray<VulkanUniformBuffer *RESTRICT, UniformBuffer::NumberOfUniformBuffers> uniformBuffers;

	//The Vulkan frame data.
	VulkanFrameData frameData;

	//The current command buffer.
	VulkanCommandBuffer *RESTRICT currentCommandBuffer;

	//The current dynamic uniform data descriptor set.
	VulkanDescriptorSet *RESTRICT currentDynamicUniformDataDescriptorSet;

	//The sky box cude map texture.
	VulkanCubeMapTexture *RESTRICT skyBoxTexture;

	//The sky box descriptor set.
	VulkanDescriptorSet skyBoxDescriptorSet;

	/*
	*	Initializes all render targets.
	*/
	void InitializeRenderTargets() NOEXCEPT;

	/*
	*	Initializes all semaphores.
	*/
	void InitializeSemaphores() NOEXCEPT;

	/*
	*	Initializes all uniform buffers.
	*/
	void InitializeUniformBuffers() NOEXCEPT;

	/*
	*	Initializes all descriptor set layouts.
	*/
	void InitializeDescriptorSetLayouts() NOEXCEPT;

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() NOEXCEPT;

	/*
	*	Initializes all pipelines.
	*/
	void InitializePipelines() NOEXCEPT;

	/*
	*	Initializes all descriptor sets.
	*/
	void InitializeDescriptorSets() NOEXCEPT;

	/*
	*	Initializes all default textures.
	*/
	void InitializeDefaultTextures() NOEXCEPT;

	/*
	*	Calculates the projection matrix.
	*/
	void CalculateProjectionMatrix() NOEXCEPT;

	/*
	*	Begins the frame.
	*/
	void BeginFrame() NOEXCEPT;

	/*
	*	Renders the terrain.
	*/
	void RenderTerrain() NOEXCEPT;

	/*
	*	Renders all static physical entities.
	*/
	void RenderStaticPhysicalEntities() NOEXCEPT;

	/*
	*	Renders all instanced physical entities.
	*/
	void RenderInstancedPhysicalEntities() NOEXCEPT;

	/*
	*	Renders lighting.
	*/
	void RenderLighting() NOEXCEPT;

	/*
	*	Renders water.
	*/
	void RenderWater() NOEXCEPT;

	/*
	*	Renders sky box.
	*/
	void RenderSkyBox() NOEXCEPT;

	/*
	*	Renders the post processing.
	*/
	void RenderPostProcessing() NOEXCEPT;

	/*
	*	Ends the frame.
	*/
	void EndFrame() NOEXCEPT;

	/*
	*	Re-initializes all descriptor sets.
	*/
	void ReinitializeDescriptorSets() NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() NOEXCEPT;

	/*
	*	Updates the view frustum culling.
	*/
	void UpdateViewFrustumCulling() NOEXCEPT;

};