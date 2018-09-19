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
#include <Rendering/Translation/Vulkan/VulkanRenderPassMainStageData.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassSubStageData.h>

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
	*	Pre-initializes the Vulkan rendering system.
	*/
	void PreInitializeSystem() NOEXCEPT;

	/*
	*	Post-initializes the Vulkan rendering system.
	*/
	void PostInitializeSystem() NOEXCEPT;

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
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Destroys a texture 2D
	*/
	void DestroyTexture2D(Texture2DHandle handle) NOEXCEPT;

	/*
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

	/*
	*	Creates a depth buffer.
	*/
	DepthBufferHandle CreateDepthBuffer(const Resolution resolution) const NOEXCEPT;

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
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle handle) NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, const TextureFilter filter, const AddressMode addressMode, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

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
	void DestroyUniformBuffer(UniformBufferHandle handle) NOEXCEPT;

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
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT;

private:

	//Enumeration covering all semaphores.
	enum class GraphicsSemaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	/*
	*	Vulkan destruction data definition.
	*/
	class VulkanDestructionData final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			RenderDataTable,
			Texture2D,
			UniformBuffer
		};

		//The number of frames since destruction was requested.
		uint8 _Frames{ 0 };

		//The type.
		Type _Type;

		//The handle.
		OpaqueHandle _Handle;

		/*
		*	Constructor taking all values as arguments.
		*/
		VulkanDestructionData(const Type type, OpaqueHandle handle) NOEXCEPT
			:
		_Type(type),
			_Handle(handle)
		{

		}

	};

	//The dynamic uniform data.
	VulkanDynamicUniformData _DynamicUniformData;

	//Container for all temporary pipelines.
	StaticArray<VulkanPipeline *RESTRICT, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)> _Pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, UNDERLYING(GraphicsSemaphore::NumberOfSemaphores)> _Semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, UNDERLYING(Shader::NumberOfShaders)> _ShaderModules;

	//Container for all Vulkan render pass main stage data.
	StaticArray<VulkanRenderPassMainStageData, UNDERLYING(RenderPassMainStage::NumberOfRenderPassMainStages)> _VulkanRenderPassMainStageData;

	//Container for all Vulkan render pass data.
	StaticArray<VulkanRenderPassSubStageData, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)> _VulkanRenderPassSubStageData;

	//The Vulkan frame data.
	VulkanFrameData _FrameData;

	//The destruction queue.
	DynamicArray<VulkanDestructionData> _DestructionQueue;

	/*
	*	Initializes all semaphores.
	*/
	void InitializeSemaphores() NOEXCEPT;

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() NOEXCEPT;

	/*
	*	Initializes all Vulkan render passes.
	*/
	void InitializeVulkanRenderPasses() NOEXCEPT;

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT;

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