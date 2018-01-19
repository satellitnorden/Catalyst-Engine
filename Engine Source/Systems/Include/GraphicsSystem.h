#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <DynamicUniformData.h>
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

class GraphicsSystem
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
	*	Pre-initializes the graphics system.
	*/
	void PreInitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Post-initializes the graphics system.
	*/
	void PostInitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the graphics system synchronously.
	*/
	void UpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Updates the graphics system asynchronously.
	*/
	void UpdateSystemAsynchronous() CATALYST_NOEXCEPT;

	/*
	*	Releases the graphics system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Creates a physical descriptor set.
	*/
	void CreatePhysicalDescriptorSet(VulkanDescriptorSet &vulkanDescriptorSet, const VulkanUniformBuffer *CATALYST_RESTRICT modelDataUniformBuffer, const VulkanTexture *CATALYST_RESTRICT albedoTexture, const VulkanTexture *CATALYST_RESTRICT normalMapTexture, const VulkanTexture *CATALYST_RESTRICT roughnessTexture, const VulkanTexture *CATALYST_RESTRICT metallicTexture, const VulkanTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns physical model.
	*/
	const PhysicalModel CreatePhysicalModel(const char *CATALYST_RESTRICT modelPath, VulkanTexture *CATALYST_RESTRICT albedoTexture, VulkanTexture *CATALYST_RESTRICT normalMapTexture, VulkanTexture *CATALYST_RESTRICT roughnessTexture, VulkanTexture *CATALYST_RESTRICT, VulkanTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a texture.
	*/
	CATALYST_RESTRICTED VulkanTexture* CreateTexture(const char *CATALYST_RESTRICT texturePath) const CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	CATALYST_RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const size_t uniformBufferSize) const CATALYST_NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *CATALYST_RESTRICT newActiveCamera) CATALYST_NOEXCEPT;

private:

	//Enumeration covering all pipelines.
	enum Pipeline : uint8
	{
		PhysicalPipeline,
		NumberOfPipelines
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
		PhysicalVertexShaderModule,
		PhysicalFragmentShaderModule,
		NumberOfShaderModules
	};

	//Enumeration covering all uniform buffers.
	enum UniformBuffer : uint8
	{
		DynamicUniformDataBuffer,
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

	//Container for all pipelines.
	VulkanPipeline *CATALYST_RESTRICT pipelines[Pipeline::NumberOfPipelines];

	//Container for all semaphores.
	VulkanSemaphore *CATALYST_RESTRICT semaphores[Semaphore::NumberOfSemaphores];

	//Container for all shader modules.
	VulkanShaderModule *CATALYST_RESTRICT shaderModules[ShaderModule::NumberOfShaderModules];

	//Container for all uniform buffers.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffers[UniformBuffer::NumberOfUniformBuffers];

	//Container for all default textures.
	VulkanTexture *CATALYST_RESTRICT defaultTextures[DefaultTexture::NumberOfDefaultTextures];

	//Container for all command buffers.
	DynamicArray<VulkanCommandBuffer> commandBuffers;

	//The current command buffer.
	size_t currentCommandBuffer{ 0 };

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() CATALYST_NOEXCEPT;

	/*
	*	Initializes all pipelines.
	*/
	void InitializePipelines() CATALYST_NOEXCEPT;

	/*
	*	Calculates the projection matrix.
	*/
	void CalculateProjectionMatrix() CATALYST_NOEXCEPT;

	/*
	*	Begins the frame.
	*/
	void BeginFrame() CATALYST_NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() CATALYST_NOEXCEPT;

	/*
	*	Renders all physical entities.
	*/
	void RenderPhysicalEntities() CATALYST_NOEXCEPT;

	/*
	*	Ends the frame.
	*/
	void EndFrame() CATALYST_NOEXCEPT;

};