#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>


//Math.
#include <Math/Matrix4.h>

//Multithreading.
#include <Multithreading/MultithreadedResource.h>
#include <Multithreading/Semaphore.h>

//Rendering.
#include <Rendering/Engine Layer/EnvironmentMaterial.h>
#include <Rendering/Engine Layer/PostProcessingUniformData.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/Window.h>
#include <Rendering/Translation Layer/Vulkan/VulkanDynamicUniformData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanFrameData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderPassData.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class EnvironmentMaterial;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class ParticleMaterial;
class ParticleSystemEntity;
class ParticleSystemProperties;
class PhysicalModel;
class RenderPass;
class Resolution;
class StaticPhysicalEntity;
class TerrainEntity;
class TerrainMaterial;
class TerrainUniformData;
class TextureData;
class VegetationEntity;
class VegetationMaterial;
class VegetationProperties;
class VegetationTransformation;
class WaterEntity;
class WaterUniformData;

class VulkanRenderingSystem final
{

public:

	//Singletone declaration.
	DECLARE_SINGLETON(VulkanRenderingSystem);

	/*
	*	Default constructor.
	*/
	VulkanRenderingSystem() NOEXCEPT = default;

	/*
	*	Initializes the Vulkan rendering system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Pre-updates the Vulkan rendering system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Post-updates the Vulkan rendering system synchronously.
	*/
	void PostUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the Vulkan rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the render resolution.
	*/
	Resolution GetRenderResolution() const NOEXCEPT;

	/*
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

	/*
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture2D &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT;

	/*
	*	Initializes a static physical entity.
	*/
	void InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Initializes a vegetation entity.
	*/
	void InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT;

	/*
	*	Initializes a particle system entity.
	*/
	void InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

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

	friend class RenderingSystem;

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	RenderDataTableHandle GetCurrentDynamicUniformDataDescriptorSet() NOEXCEPT;

	/*
	*	Returns the current environment data descriptor set.
	*/
	RenderDataTableHandle GetCurrentEnvironmentDataDescriptorSet() NOEXCEPT;

	/*
	*	Returns the current ocean descriptor set.
	*/
	RenderDataTableHandle GetCurrentOceanDescriptorSet() NOEXCEPT;

	/*
	*	Returns the directional shadow descriptor set.
	*/
	RenderDataTableHandle GetDirectionalShadowDescriptorSet() NOEXCEPT;

	/*
	*	Returns the lighting descriptor set.
	*/
	RenderDataTableHandle GetLightingDescriptorSet() NOEXCEPT;

	/*
	*	Returns the post processing descriptor set.
	*/
	RenderDataTableHandle GetPostProcessingDescriptorSet() NOEXCEPT;

	/*
	*	Returns the shadow blur descriptor set.
	*/
	RenderDataTableHandle GetShadowBlurDescriptorSet() NOEXCEPT;

	//Enumeration covering all default textures.
	enum DefaultTexture : uint8
	{
		Black,
		White,
		NumberOfDefaultTextures
	};

	//Enumeration covering all descriptor sets.
	enum class DescriptorSet : uint8
	{
		Environment,
		ShadowBlur,
		DirectionalShadow,
		Lighting,
		PostProcessing,
		NumberOfDescriptorSets
	};

	//Enumeration covering all semaphores.
	enum class GraphicsSemaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	//Enumeration covering all task semaphores.
	enum class TaskSemaphore : uint8
	{
		UpdateParticleSystemProperties,
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

	//The dynamic uniform data.
	VulkanDynamicUniformData dynamicUniformData;

	//The post processing uniform data.
	PostProcessingUniformData postProcessingUniformData;

	//Container for all default textures.
	StaticArray<Vulkan2DTexture *RESTRICT, DefaultTexture::NumberOfDefaultTextures> defaultTextures;

	//Container for all depth buffers.
	StaticArray<VulkanDepthBuffer *RESTRICT, INDEX(DepthBuffer::NumberOfDepthBuffers)> depthBuffers;

	//Container for all descriptor sets.
	StaticArray<VulkanDescriptorSet, INDEX(DescriptorSet::NumberOfDescriptorSets)> descriptorSets;

	//Container for all descriptor set layouts.
	StaticArray<VulkanDescriptorSetLayout, INDEX(DescriptorSetLayout::NumberOfDescriptorSetLayouts)> descriptorSetLayouts;

	//Container for all render targets.
	StaticArray<VulkanRenderTarget *RESTRICT, INDEX(RenderTarget::NumberOfRenderTargets)> renderTargets;

	//Container for all temporary pipelines.
	StaticArray<VulkanPipeline *RESTRICT, INDEX(RenderPassStage::NumberOfRenderPassStages)> pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, INDEX(GraphicsSemaphore::NumberOfSemaphores)> semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, INDEX(Shader::NumberOfShaders)> shaderModules;

	//Container for all uniform buffers.
	StaticArray<VulkanUniformBuffer *RESTRICT, UniformBuffer::NumberOfUniformBuffers> uniformBuffers;

	//Container for all Vulkan render pass data.
	StaticArray<VulkanRenderPassData, INDEX(RenderPassStage::NumberOfRenderPassStages)> vulkanRenderPassData;

	//The Vulkan frame data.
	VulkanFrameData frameData;

	//The current dynamic uniform data descriptor set.
	VulkanDescriptorSet *RESTRICT currentDynamicUniformDataDescriptorSet;

	//The current environment descriptor set.
	VulkanDescriptorSet *RESTRICT currentEnvironmentDataDescriptorSet;

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
	*	Initializes all descriptor sets.
	*/
	void InitializeDescriptorSets() NOEXCEPT;

	/*
	*	Initializes all default textures.
	*/
	void InitializeDefaultTextures() NOEXCEPT;

	/*
	*	Execute asynchronous tasks.
	*/
	void ExecuteAsynchronousTasks() NOEXCEPT;

	/*
	*	Begins the frame.
	*/
	void BeginFrame() NOEXCEPT;

	/*
	*	Concatenates all secondary command buffers into the previous one.
	*/
	void ConcatenateCommandBuffers() NOEXCEPT;

	/*
	*	Ends the frame.
	*/
	void EndFrame() NOEXCEPT;

	/****************************/
	/*	Asynchronous functions. */
	/****************************/

	/*
	*	Updates the properties of particle systems.
	*/
	void UpdateParticleSystemProperties() const NOEXCEPT;

	/*
	*	Updates the descriptor sets.
	*/
	void UpdateDescriptorSets() NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() NOEXCEPT;

};