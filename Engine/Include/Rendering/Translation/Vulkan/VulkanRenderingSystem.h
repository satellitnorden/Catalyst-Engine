#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Multithreading.
#include <Multithreading/MultithreadedResource.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Translation/Vulkan/VulkanDynamicUniformData.h>
#include <Rendering/Translation/Vulkan/VulkanFrameData.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassData.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class Entity;
class EnvironmentMaterial;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class ParticleMaterial;
class ParticleSystemEntity;
class ParticleSystemInitializationData;
class ParticleSystemProperties;
class PhysicalModel;
class RenderPass;
class Resolution;
class TerrainEntity;
class TerrainInitializationData;
class TerrainMaterial;
class TerrainUniformData;
class TextureData;

class VulkanRenderingSystem final
{

public:

	//Singletone declaration.
	DECLARE_SINGLETON(VulkanRenderingSystem);

	/*
	*	Default constructor.
	*/
	VulkanRenderingSystem() NOEXCEPT { }

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
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

	/*
	*	Returns the given render target.
	*/
	RenderTargetHandle GetRenderTarget(const RenderTarget renderTarget) NOEXCEPT;

	/*
	*	Returns the given uniform buffer.
	*/
	UniformBufferHandle GetUniformBuffer(const UniformBuffer uniformBuffer) NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Destroys a texture 2D
	*/
	void DestroyTexture2D(Texture2DHandle texture) const NOEXCEPT;

	/*
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle renderDataTable) const NOEXCEPT;

	/*
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Uploads data to a uniform buffer.
	*/
	void UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT;

	/*
	*	Destroys a uniform buffer.
	*/
	void DestroyUniformBuffer(UniformBufferHandle handle) const NOEXCEPT;

private:

	friend class RenderingSystem;

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT;

	/*
	*	Creates a render data table layout.
	*/
	void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table.
	*/
	void CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Updates a render data table.
	*/
	void UpdateRenderDataTable(const RenderDataTableUpdateInformation information, RenderDataTableHandle handle) const NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	RenderDataTableHandle GetCurrentDynamicUniformDataRenderDataTable() NOEXCEPT;

	/*
	*	Returns the current environment data descriptor set.
	*/
	RenderDataTableHandle GetCurrentEnvironmentRenderDataTable() NOEXCEPT;

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Returns the current ocean descriptor set.
	*/
	RenderDataTableHandle GetCurrentOceanRenderDataTable() NOEXCEPT;
#endif

	/*
	*	Returns the given common render data table layout.
	*/
	RenderDataTableHandle GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) NOEXCEPT;

	//Enumeration covering all semaphores.
	enum class GraphicsSemaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	class VulkanRenderPassMainStageData final
	{

	public:

		//The render pass.
		VulkanRenderPass *RESTRICT _RenderPass;

		//The framebuffers.
		DynamicArray<VulkanFramebuffer *RESTRICT> _FrameBuffers;

		//Defines whether or not to clear the framebuffers.
		bool _ShouldClear;

		//The number of attachments.
		uint32 _NumberOfAttachments;

	};

	//The dynamic uniform data.
	VulkanDynamicUniformData _DynamicUniformData;

	//Container for all depth buffers.
	StaticArray<VulkanDepthBuffer *RESTRICT, INDEX(DepthBuffer::NumberOfDepthBuffers)> _DepthBuffers;

	//Container for all descriptor set layouts.
	StaticArray<VulkanDescriptorSetLayout, INDEX(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts)> _DescriptorSetLayouts;

	//Container for all render targets.
	StaticArray<VulkanRenderTarget *RESTRICT, INDEX(RenderTarget::NumberOfRenderTargets)> _RenderTargets;

	//Container for all temporary pipelines.
	StaticArray<VulkanPipeline *RESTRICT, INDEX(RenderPassSubStage::NumberOfRenderPassSubStages)> _Pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, INDEX(GraphicsSemaphore::NumberOfSemaphores)> _Semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, INDEX(Shader::NumberOfShaders)> _ShaderModules;

	//Container for all uniform buffers.
	StaticArray<VulkanUniformBuffer *RESTRICT, INDEX(UniformBuffer::NumberOfUniformBuffers)> _UniformBuffers;

	//Container for all special textures.
	StaticArray<Vulkan2DTexture* RESTRICT, INDEX(SpecialTexture::NumberOfSpecialTextures)> _SpecialTextures;

	//Container for all Vulkan render pass main stage data.
	StaticArray<VulkanRenderPassMainStageData, INDEX(RenderPassMainStage::NumberOfRenderPassMainStages)> _VulkanRenderPassMainStageData;

	//Container for all Vulkan render pass data.
	StaticArray<VulkanRenderPassData, INDEX(RenderPassSubStage::NumberOfRenderPassSubStages)> _VulkanRenderPassData;

	//The Vulkan frame data.
	VulkanFrameData _FrameData;

	//The current dynamic uniform data descriptor set.
	VulkanDescriptorSet *RESTRICT _CurrentDynamicUniformDataDescriptorSet;

	//The current environment descriptor set.
	VulkanDescriptorSet *RESTRICT _CurrentEnvironmentDataDescriptorSet;

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
	*	Initializes all Vulkan render passes.
	*/
	void InitializeVulkanRenderPasses() NOEXCEPT;

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

	/*
	*	Updates the descriptor sets.
	*/
	void UpdateDescriptorSets() NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() NOEXCEPT;

};