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
class PhysicalModel;

class GraphicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GraphicsSystem);

	/*
	*	Default constructor.
	*/
	GraphicsSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~GraphicsSystem() CATALYST_NOEXCEPT;

	/*
	*	Initializes the graphics system.
	*/
	void InitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Post-initializes the graphics system.
	*/
	void PostInitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the graphics system synchronously.
	*/
	void UpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Releases the graphics system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Creates a physical descriptor set.
	*/
	void CreatePhysicalDescriptorSet(VulkanDescriptorSet &vulkanDescriptorSet, const VulkanUniformBuffer *CATALYST_RESTRICT modelDataUniformBuffer, const Vulkan2DTexture *CATALYST_RESTRICT albedoTexture, const Vulkan2DTexture *CATALYST_RESTRICT normalMapTexture, const Vulkan2DTexture *CATALYST_RESTRICT roughnessTexture, const Vulkan2DTexture *CATALYST_RESTRICT metallicTexture, const Vulkan2DTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns physical model.
	*/
	const PhysicalModel CreatePhysicalModel(const char *CATALYST_RESTRICT modelPath, Vulkan2DTexture *CATALYST_RESTRICT albedoTexture, Vulkan2DTexture *CATALYST_RESTRICT normalMapTexture, Vulkan2DTexture *CATALYST_RESTRICT roughnessTexture, Vulkan2DTexture *CATALYST_RESTRICT, Vulkan2DTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a 2D texture.
	*/
	CATALYST_RESTRICTED Vulkan2DTexture* Create2DTexture(const char *CATALYST_RESTRICT texturePath) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	CATALYST_RESTRICTED VulkanCubeMapTexture* CreateCubeMapTexture(const char *CATALYST_RESTRICT leftTexturePath, const char *CATALYST_RESTRICT rightTexturePath, const char *CATALYST_RESTRICT downTexturePath, const char *CATALYST_RESTRICT upTexturePath, const char *CATALYST_RESTRICT backTexturePath, const char *CATALYST_RESTRICT frontTexturePath) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	CATALYST_RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const size_t uniformBufferSize) const CATALYST_NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *CATALYST_RESTRICT newActiveCamera) CATALYST_NOEXCEPT;

	/*
	*	Sets the active sky box cube map texture.
	*/
	void SetActiveSkyBox(VulkanCubeMapTexture *CATALYST_RESTRICT newSkyBox) CATALYST_NOEXCEPT;

	/*
	*	Sets the post processing blur amount.
	*/
	void SetPostProcessingBlurAmount(const float newBlurAmount) CATALYST_NOEXCEPT;

	/*
	*	Sets the post processing chromatic aberration amount.
	*/
	void SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) CATALYST_NOEXCEPT;

	/*
	*	Sets the post processing saturation.
	*/
	void SetPostProcessingSaturation(const float newSaturation) CATALYST_NOEXCEPT;

	/*
	*	Sets the post processing sharpen amount.
	*/
	void SetPostProcessingSharpenAmount(const float newSharpenAmount) CATALYST_NOEXCEPT;

private:

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

	//Enumeration covering all default textures.
	enum DefaultTexture : uint8
	{
		Roughness,
		Metallic,
		AmbientOcclusion,
		NumberOfDefaultTextures
	};

	//The main window.
	Window mainWindow;

	//The active camera.
	CameraEntity *CATALYST_RESTRICT activeCamera;

	//The current projection matrix.
	Matrix4 projectionMatrix;

	//The current view matrix.
	MultithreadedResource<Matrix4> viewMatrix;

	//The dynamic uniform data.
	DynamicUniformData dynamicUniformData;

	//The post processing uniform data.
	PostProcessingUniformData postProcessingUniformData;

	//Container for all depth buffers.
	VulkanDepthBuffer *CATALYST_RESTRICT depthBuffers[DepthBuffer::NumberOfDepthBuffers];

	//Container for all descriptor sets.
	VulkanDescriptorSet descriptorSets[DescriptorSet::NumberOfDescriptorSet];

	//Container for all render targets.
	VulkanRenderTarget *CATALYST_RESTRICT renderTargets[RenderTarget::NumberOfRenderTargets];

	//Container for all pipelines.
	VulkanPipeline *CATALYST_RESTRICT pipelines[Pipeline::NumberOfPipelines];

	//Container for all semaphores.
	VulkanSemaphore *CATALYST_RESTRICT semaphores[Semaphore::NumberOfSemaphores];

	//Container for all shader modules.
	VulkanShaderModule *CATALYST_RESTRICT shaderModules[ShaderModule::NumberOfShaderModules];

	//Container for all uniform buffers.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffers[UniformBuffer::NumberOfUniformBuffers];

	//Container for all default textures.
	Vulkan2DTexture *CATALYST_RESTRICT defaultTextures[DefaultTexture::NumberOfDefaultTextures];

	//Container for all swapchain command buffers.
	DynamicArray<VulkanCommandBuffer> swapchainCommandBuffers;

	//The current swapchain command buffer.
	size_t currentSwapchainCommandBuffer{ 0 };

	//The sky box cude map texture.
	VulkanCubeMapTexture *CATALYST_RESTRICT skyBoxTexture;

	//The sky box descriptor set.
	VulkanDescriptorSet skyBoxDescriptorSet;

	/*
	*	Initializes all render targets.
	*/
	void InitializeRenderTargets() CATALYST_NOEXCEPT;

	/*
	*	Initializes all uniform buffers.
	*/
	void InitializeUniformBuffers() CATALYST_NOEXCEPT;

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() CATALYST_NOEXCEPT;

	/*
	*	Initializes all pipelines.
	*/
	void InitializePipelines() CATALYST_NOEXCEPT;

	/*
	*	Initializes all descriptor sets.
	*/
	void InitializeDescriptorSets() CATALYST_NOEXCEPT;

	/*
	*	Calculates the projection matrix.
	*/
	void CalculateProjectionMatrix() CATALYST_NOEXCEPT;

	/*
	*	Begins the frame.
	*/
	void BeginFrame() CATALYST_NOEXCEPT;

	/*
	*	Renders all physical entities.
	*/
	void RenderPhysicalEntities() CATALYST_NOEXCEPT;

	/*
	*	Renders lighting.
	*/
	void RenderLighting() CATALYST_NOEXCEPT;

	/*
	*	Renders sky box.
	*/
	void RenderSkyBox() CATALYST_NOEXCEPT;

	/*
	*	Renders the post processing.
	*/
	void RenderPostProcessing() CATALYST_NOEXCEPT;

	/*
	*	Ends the frame.
	*/
	void EndFrame() CATALYST_NOEXCEPT;

	/*
	*	Re-initializes all descriptor sets.
	*/
	void ReinitializeDescriptorSets() CATALYST_NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() CATALYST_NOEXCEPT;

	/*
	*	Updates the view frustum culling.
	*/
	void UpdateViewFrustumCulling() CATALYST_NOEXCEPT;

};