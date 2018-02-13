#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <DynamicUniformData.h>
#include <PostProcessingUniformData.h>
#include <Window.h>

//Math.
#include <Matrix4.h>

//Multithreading.
#include <MultithreadedResource.h>

//Vulkan.
#include <VulkanInterface.h>

//Forward declarations.
class CameraEntity;
class PhysicalEntity;
class PhysicalModel;
class TerrainEntity;
class TerrainUniformData;
class TextureData;

class GraphicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GraphicsSystem);

	/*
	*	Default constructor.
	*/
	GraphicsSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~GraphicsSystem() NOEXCEPT;

	/*
	*	Initializes the graphics system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Post-initializes the graphics system.
	*/
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Updates the graphics system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the graphics system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Creates and returns physical model.
	*/
	const PhysicalModel CreatePhysicalModel(const char *RESTRICT modelPath, Vulkan2DTexture *RESTRICT albedoTexture, Vulkan2DTexture *RESTRICT normalMapTexture, Vulkan2DTexture *RESTRICT roughnessTexture, Vulkan2DTexture *RESTRICT, Vulkan2DTexture *RESTRICT ambientOcclusionTexture) const NOEXCEPT;

	/*
	*	Initializes a physical entity.
	*/
	void InitializePhysicalEntity(PhysicalEntity &physicalEntity, const PhysicalModel &model) const NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Vulkan2DTexture *RESTRICT terrainHeightMapTexture, const Vulkan2DTexture *RESTRICT terrainNormalMapTexture, const Vulkan2DTexture *RESTRICT layer1WeightTexture, const Vulkan2DTexture *RESTRICT layer1AlbedoTexture, const Vulkan2DTexture *RESTRICT layer1NormalMapTexture, const Vulkan2DTexture *RESTRICT layer1RoughnessTexture, const Vulkan2DTexture *RESTRICT layer1MetallicTexture, const Vulkan2DTexture *RESTRICT layer1AmbientOcclusionTexture, const Vulkan2DTexture *RESTRICT layer1DisplacementTexture, const Vulkan2DTexture *RESTRICT layer2WeightTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2AlbedoTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2NormalMapTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2RoughnessTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2MetallicTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2AmbientOcclusionTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2DisplacementTexture = nullptr) const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	RESTRICTED Vulkan2DTexture* Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	RESTRICTED VulkanCubeMapTexture* CreateCubeMapTexture(const char *RESTRICT leftTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT frontTexturePath) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const size_t uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT;

	/*
	*	Sets the active sky box cube map texture.
	*/
	void SetActiveSkyBox(VulkanCubeMapTexture *RESTRICT newSkyBox) NOEXCEPT;

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

	//Enumeration covering all command buffers.
	enum CommandBuffer : uint8
	{
		SceneBufferCommandBuffer,
		NumberOfCommandBuffers
	};

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

	//Enumeration covering all pipelines.
	enum Pipeline : uint8
	{
		TerrainSceneBufferPipeline,
		SceneBufferPipeline,
		LightingPipeline,
		CubeMapPipeline,
		PostProcessingPipeline,
		NumberOfPipelines
	};

	//Enumeration covering all render targets.
	enum RenderTarget : uint8
	{
		SceneBufferAlbedoColorRenderTarget,
		SceneBufferNormalDirectionDepthRenderTarget,
		SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget,
		SceneRenderTarget,
		NumberOfRenderTargets
	};

	//Enumeration covering all semaphores.
	enum Semaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	//Enumeration covering all shader modules.
	enum ShaderModule : uint8
	{
		CubeMapFragmentShaderModule,
		CubeMapVertexShaderModule,
		LightingFragmentShaderModule,
		PostProcessingFragmentShaderModule,
		SceneBufferFragmentShaderModule,
		SceneBufferVertexShaderModule,
		TerrainSceneBufferFragmentShaderModule,
		TerrainSceneBufferTessellationControlShaderModule,
		TerrainSceneBufferTessellationEvaluationShaderModule,
		TerrainSceneBufferVertexShaderModule,
		ViewportVertexShaderModule,
		NumberOfShaderModules
	};

	//Enumeration covering all uniform buffers.
	enum UniformBuffer : uint8
	{
		DynamicUniformDataBuffer,
		PostProcessingUniformDataBuffer,
		NumberOfUniformBuffers
	};

	//The main window.
	Window mainWindow;

	//The active camera.
	CameraEntity *RESTRICT activeCamera;

	//The current projection matrix.
	Matrix4 projectionMatrix;

	//The dynamic uniform data.
	DynamicUniformData dynamicUniformData;

	//The post processing uniform data.
	PostProcessingUniformData postProcessingUniformData;

	//Container for all command buffers.
	StaticArray<VulkanCommandBuffer, CommandBuffer::NumberOfCommandBuffers> commandBuffers;

	//Container for all default textures.
	StaticArray<Vulkan2DTexture *RESTRICT, DefaultTexture::NumberOfDefaultTextures> defaultTextures;

	//Container for all depth buffers.
	StaticArray<VulkanDepthBuffer *RESTRICT, DepthBuffer::NumberOfDepthBuffers> depthBuffers;

	//Container for all descriptor sets.
	StaticArray<VulkanDescriptorSet, DescriptorSet::NumberOfDescriptorSet> descriptorSets;

	//Container for all render targets.
	StaticArray<VulkanRenderTarget *RESTRICT, RenderTarget::NumberOfRenderTargets> renderTargets;

	//Container for all pipelines.
	StaticArray<VulkanPipeline *RESTRICT, Pipeline::NumberOfPipelines> pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, Semaphore::NumberOfSemaphores> semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, ShaderModule::NumberOfShaderModules> shaderModules;

	//Container for all uniform buffers.
	StaticArray<VulkanUniformBuffer *RESTRICT, UniformBuffer::NumberOfUniformBuffers> uniformBuffers;

	//Container for all swapchain command buffers.
	DynamicArray<VulkanCommandBuffer> swapchainCommandBuffers;

	//The current swapchain command buffer.
	size_t currentSwapchainCommandBuffer{ 0 };

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
	*	Renders all physical entities.
	*/
	void RenderPhysicalEntities() NOEXCEPT;

	/*
	*	Renders lighting.
	*/
	void RenderLighting() NOEXCEPT;

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
	*	Updates the physical entities graphics buffers.
	*/
	void UpdatePhysicalEntitiesGraphicsBuffers() NOEXCEPT;

	/*
	*	Updates the view frustum culling.
	*/
	void UpdateViewFrustumCulling() NOEXCEPT;

};