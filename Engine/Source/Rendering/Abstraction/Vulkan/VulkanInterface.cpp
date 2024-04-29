#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/Spinlock.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanMemoryAllocator.h>
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//Systems.
#include <Systems/MemorySystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanErrorReporting.h>

/*
*	Initializes this Vulkan interface.
*/
void VulkanInterface::Initialize() NOEXCEPT
{
#if DEFINE_FUNCTION_PROTOTYPES
	DefineFunctionPrototypes();
#endif

	//Initialize the Vulkan instance.
	_VulkanInstance.Initialize();

#if VULKAN_DEBUGGING && !defined(CATALYST_PLATFORM_ANDROID) && !defined(CATALYST_PLATFORM_OCULUS_QUEST)
	//Initialize the Vulkan error reporting.
	VulkanErrorReporting::Initialize();
#endif

	//Initialize the Vulkan surface.
	_VulkanSurface.Initialize();

	//Initialize the Vulkan physical device.
	_VulkanPhysicalDevice.Initialize();

	//Initialize the Vulkan logical device.
	_VulkanLogicalDevice.Initialize();

	//Tell the platform that the logical device has been created.
	VulkanPlatform::OnLogicalDeviceCreated();

	//Create the Vulkan memory allocator.
	CreateVulkanMemoryAllocator();

	//Initialize the Vulkan swap chain.
	_VulkanSwapchain.Initialize();

	//Initialize the Vulkan descriptor pool.
	_VulkanDescriptorPool.Initialize();
}

/*
*	Pre-updates this Vulkan interface.
*/
void VulkanInterface::PreUpdate(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT
{
	//Update the next image index in the Vulkan swap chain.
	_VulkanSwapchain.UpdateNextImageIndex(imageAvailableSemaphore);
}

/*
*	Post-pdates this Vulkan interface.
*/
void VulkanInterface::PostUpdate(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT
{
	//Present the final image!
	_VulkanSwapchain.Present(renderFinishedSemaphore);
}

/*
*	Releases this Vulkan interface.
*/
void VulkanInterface::Release() NOEXCEPT
{
	//Wait for all queues to finish.
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::MAIN)->WaitIdle();
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::ASYNC_COMPUTE)->WaitIdle();
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::ASYNC_TRANSFER)->WaitIdle();

	//Tell the platform to terminate
	VulkanPlatform::Terminate();

	//Release all Vulkan 2D textures.
	for (Vulkan2DTexture *const RESTRICT vulkan_2D_texture : _Vulkan2DTextures)
	{
		vulkan_2D_texture->Release();
		vulkan_2D_texture->~Vulkan2DTexture();
		MemorySystem::Instance->TypeFree<Vulkan2DTexture>(vulkan_2D_texture);
	}

	//Release all Vulkan 3D textures.
	for (Vulkan3DTexture* const RESTRICT vulkan_3D_texture : _Vulkan3DTextures)
	{
		vulkan_3D_texture->Release();
		vulkan_3D_texture->~Vulkan3DTexture();
		MemorySystem::Instance->TypeFree<Vulkan3DTexture>(vulkan_3D_texture);
	}

	//Release all Vulkan acceleration structures.
	for (VulkanAccelerationStructure *const RESTRICT vulkan_acceleration_structure : _VulkanAccelerationStructures)
	{
		vulkan_acceleration_structure->Release();
		vulkan_acceleration_structure->~VulkanAccelerationStructure();
		MemorySystem::Instance->TypeFree<VulkanAccelerationStructure>(vulkan_acceleration_structure);
	}

	//Release all Vulkan bufferrs.
	for (VulkanBuffer *const RESTRICT vulkan_buffer : _VulkanBuffers)
	{
		vulkan_buffer->Release();
		vulkan_buffer->~VulkanBuffer();
		MemorySystem::Instance->TypeFree<VulkanBuffer>(vulkan_buffer);
	}

	//Release all Vulkan command pools.
	for (VulkanCommandPool *const RESTRICT vulkan_command_pool : _VulkanCommandPools)
	{
		vulkan_command_pool->Release();
		vulkan_command_pool->~VulkanCommandPool();
		MemorySystem::Instance->TypeFree<VulkanCommandPool>(vulkan_command_pool);
	}

	//Release all Vulkan cube map textures.
	for (VulkanCubeMapTexture *const RESTRICT vulkan_cube_map_texture : _VulkanCubeMapTextures)
	{
		vulkan_cube_map_texture->Release();
		vulkan_cube_map_texture->~VulkanCubeMapTexture();
		MemorySystem::Instance->TypeFree<VulkanCubeMapTexture>(vulkan_cube_map_texture);
	}

	//Release all Vulkan depth buffers.
	for (VulkanDepthBuffer *const RESTRICT vulkan_depth_buffer : _VulkanDepthBuffers)
	{
		vulkan_depth_buffer->Release();
		vulkan_depth_buffer->~VulkanDepthBuffer();
		MemorySystem::Instance->TypeFree<VulkanDepthBuffer>(vulkan_depth_buffer);
	}

	//Release all Vulkan descriptor set layouts.
	for (VulkanDescriptorSetLayout *const RESTRICT vulkan_descriptor_set_layout : _VulkanDescriptorSetLayouts)
	{
		vulkan_descriptor_set_layout->Release();
		vulkan_descriptor_set_layout->~VulkanDescriptorSetLayout();
		MemorySystem::Instance->TypeFree<VulkanDescriptorSetLayout>(vulkan_descriptor_set_layout);
	}

	//Release all Vulkan descriptor sets.
	for (VulkanDescriptorSet *const RESTRICT vulkan_descriptor_set : _VulkanDescriptorSets)
	{
		vulkan_descriptor_set->~VulkanDescriptorSet();
		MemorySystem::Instance->TypeFree<VulkanDescriptorSet>(vulkan_descriptor_set);
	}

	//Release all Vulkan events.
	for (VulkanEvent *const RESTRICT vulkan_event : _VulkanEvents)
	{
		vulkan_event->Release();
		vulkan_event->~VulkanEvent();
		MemorySystem::Instance->TypeFree<VulkanEvent>(vulkan_event);
	}

	//Release all Vulkan fences.
	for (VulkanFence *const RESTRICT vulkan_fence : _VulkanFences)
	{
		vulkan_fence->Release();
		vulkan_fence->~VulkanFence();
		MemorySystem::Instance->TypeFree<VulkanFence>(vulkan_fence);
	}

	//Release all Vulkan framebuffers.
	for (VulkanFramebuffer *const RESTRICT vulkanFramebuffer : _VulkanFramebuffers)
	{
		vulkanFramebuffer->Release();
		vulkanFramebuffer->~VulkanFramebuffer();
		MemorySystem::Instance->TypeFree<VulkanFramebuffer>(vulkanFramebuffer);
	}

	//Release all Vulkan render targets.
	for (VulkanRenderTarget *const RESTRICT vulkan_render_target : _VulkanRenderTargets)
	{
		vulkan_render_target->Release();
		vulkan_render_target->~VulkanRenderTarget();
		MemorySystem::Instance->TypeFree<VulkanRenderTarget>(vulkan_render_target);
	}

	//Release all Vulkan compute pipelines.
	for (VulkanComputePipeline *const RESTRICT vulkan_compute_pipeline : _VulkanComputePipelines)
	{
		vulkan_compute_pipeline->Release();
		vulkan_compute_pipeline->~VulkanComputePipeline();
		MemorySystem::Instance->TypeFree<VulkanComputePipeline>(vulkan_compute_pipeline);
	}

	//Release all Vulkan graphics pipelines.
	for (VulkanGraphicsPipeline *const RESTRICT vulkan_graphics_pipeline : _VulkanGraphicsPipelines)
	{
		vulkan_graphics_pipeline->Release();
		vulkan_graphics_pipeline->~VulkanGraphicsPipeline();
		MemorySystem::Instance->TypeFree<VulkanGraphicsPipeline>(vulkan_graphics_pipeline);
	}

	//Release all Vulkan ray tracing pipelines.
	for (VulkanRayTracingPipeline *const RESTRICT vulkan_ray_tracing_pipeline : _VulkanRayTracingPipelines)
	{
		vulkan_ray_tracing_pipeline->Release();
		vulkan_ray_tracing_pipeline->~VulkanRayTracingPipeline();
		MemorySystem::Instance->TypeFree<VulkanRayTracingPipeline>(vulkan_ray_tracing_pipeline);
	}

	//Release all Vulkan query pools.
	for (VulkanQueryPool *const RESTRICT vulkan_query_pool : _VulkanQueryPools)
	{
		vulkan_query_pool->Release();
		vulkan_query_pool->~VulkanQueryPool();
		MemorySystem::Instance->TypeFree<VulkanQueryPool>(vulkan_query_pool);
	}

	//Release all Vulkan render passes.
	for (VulkanRenderPass *const RESTRICT vulkan_render_pass : _VulkanRenderPasses)
	{
		vulkan_render_pass->Release();
		vulkan_render_pass->~VulkanRenderPass();
		MemorySystem::Instance->TypeFree<VulkanRenderPass>(vulkan_render_pass);
	}

	//Release all Vulkan semaphores.
	for (VulkanSemaphore *const RESTRICT vulkan_semaphore : _VulkanSemaphores)
	{
		vulkan_semaphore->Release();
		vulkan_semaphore->~VulkanSemaphore();
		MemorySystem::Instance->TypeFree<VulkanSemaphore>(vulkan_semaphore);
	}

	//Release all Vulkan samplers.
	for (VulkanSampler *const RESTRICT vulkan_sampler : _VulkanSamplers)
	{
		vulkan_sampler->Release();
		vulkan_sampler->~VulkanSampler();
		MemorySystem::Instance->TypeFree<VulkanSampler>(vulkan_sampler);
	}

	//Release all Vulkan shader modules.
	for (VulkanShaderModule *const RESTRICT vulkan_shader_module : _VulkanShaderModules)
	{
		vulkan_shader_module->Release();
		vulkan_shader_module->~VulkanShaderModule();
		MemorySystem::Instance->TypeFree<VulkanShaderModule>(vulkan_shader_module);
	}

	//Release the Vulkan descriptor pool.
	_VulkanDescriptorPool.Release();

	//Release the Vulkan swap chain.
	_VulkanSwapchain.Release();

	//Destroy the Vulkan memory allocator.
	DestroyVulkanMemoryAllocator();

	//Release the Vulkan logical device.
	_VulkanLogicalDevice.Release();

	//Release the Vulkan surface.
	_VulkanSurface.Release();

#if VULKAN_DEBUGGING
	//Release the Vulkan error reporting.
	VulkanErrorReporting::Release();
#endif

	//Release the Vulkan instance.
	_VulkanInstance.Release();
}

/*
*	Creates and returns a 2D texture.
*/
RESTRICTED Vulkan2DTexture *const RESTRICT VulkanInterface::Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkImageUsageFlags image_usage_flags, const bool updatable) NOEXCEPT
{
	Vulkan2DTexture *const RESTRICT new_vulkan_2D_texture{ new (MemorySystem::Instance->TypeAllocate<Vulkan2DTexture>()) Vulkan2DTexture() };
	new_vulkan_2D_texture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureChannels, textureTexelSize, textureData, format, image_usage_flags, updatable);

	{
		SCOPED_LOCK(_Vulkan2DTexturesLock);

		_Vulkan2DTextures.Emplace(new_vulkan_2D_texture);
	}

	return new_vulkan_2D_texture;
}

/*
*	Updates a 2D texture.
*/
void VulkanInterface::Update2DTexture(const void *RESTRICT const *RESTRICT texture_data, Vulkan2DTexture *const RESTRICT texture) NOEXCEPT
{
	texture->Update(texture_data);
}

/*
*	Destroys a 2D texture.
*/
void VulkanInterface::Destroy2DTexture(Vulkan2DTexture *const RESTRICT texture) NOEXCEPT
{
	texture->Release();
	texture->~Vulkan2DTexture();
	MemorySystem::Instance->TypeFree<Vulkan2DTexture>(texture);

	{
		SCOPED_LOCK(_Vulkan2DTexturesLock);

		_Vulkan2DTextures.Erase<false>(texture);
	}
}

/*
*	Creates and returns a 3D texture.
*/
RESTRICTED Vulkan3DTexture* const RESTRICT VulkanInterface::Create3DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureDepth, const uint32 textureChannels, const uint32 textureTexelSize, const void* RESTRICT const* RESTRICT textureData, const VkFormat format) NOEXCEPT
{
	Vulkan3DTexture* const RESTRICT new_vulkan_3D_texture{ new (MemorySystem::Instance->TypeAllocate<Vulkan3DTexture>()) Vulkan3DTexture() };
	new_vulkan_3D_texture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureDepth, textureChannels, textureTexelSize, textureData, format);

	{
		SCOPED_LOCK(_Vulkan3DTexturesLock);

		_Vulkan3DTextures.Emplace(new_vulkan_3D_texture);
	}

	return new_vulkan_3D_texture;
}

/*
*	Creates an acceleration structure.
*/
RESTRICTED VulkanAccelerationStructure *const RESTRICT VulkanInterface::CreateAccelerationStructure
(
	const VkAccelerationStructureTypeNV type,
	const ArrayProxy<VulkanGeometryInstance> &instances,
	const ArrayProxy<VkGeometryNV> &geometry,
	VulkanCommandBuffer *const RESTRICT command_buffer
) NOEXCEPT
{
	VulkanAccelerationStructure* const RESTRICT new_acceleration_structure{ new (MemorySystem::Instance->TypeAllocate<VulkanAccelerationStructure>()) VulkanAccelerationStructure() };
	new_acceleration_structure->Initialize(type, instances, geometry, command_buffer);

	{
		SCOPED_LOCK(_VulkanAccelerationStructuresLock);

		_VulkanAccelerationStructures.Emplace(new_acceleration_structure);
	}

	return new_acceleration_structure;
}

/*
*	Destroys an acceleration structure
*/
void VulkanInterface::DestroyAccelerationStructure(VulkanAccelerationStructure *const RESTRICT acceleration_structure) NOEXCEPT
{
	acceleration_structure->Release();
	
	{
		SCOPED_LOCK(_VulkanAccelerationStructuresLock);

		_VulkanAccelerationStructures.Erase<false>(acceleration_structure);
	}

	MemorySystem::Instance->TypeFree<VulkanAccelerationStructure>(acceleration_structure);
}

/*
*	Creates and returns a  buffer.
*/
RESTRICTED VulkanBuffer *const RESTRICT VulkanInterface::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT
{
	VulkanBuffer *const RESTRICT new_buffer{ new (MemorySystem::Instance->TypeAllocate<VulkanBuffer>()) VulkanBuffer() };
	new_buffer->Initialize(size, usage, memoryProperties);

	{
		SCOPED_LOCK(_VulkanBuffersLock);

		_VulkanBuffers.Emplace(new_buffer);
	}

	return new_buffer;
}

/*
*	Destroys a buffer.
*/
void VulkanInterface::DestroyBuffer(VulkanBuffer *const RESTRICT buffer) NOEXCEPT
{
	buffer->Release();
	
	{
		SCOPED_LOCK(_VulkanBuffersLock);

		_VulkanBuffers.Erase<false>(buffer);
	}

	MemorySystem::Instance->TypeFree<VulkanBuffer>(buffer);
}

/*
*	Creates and returns a command pool from the "main" queue.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateMainCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT new_commad_pool{ new (MemorySystem::Instance->TypeAllocate<VulkanCommandPool>()) VulkanCommandPool() };
	new_commad_pool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::MAIN));

	{
		SCOPED_LOCK(_VulkanCommandPoolsLock);

		_VulkanCommandPools.Emplace(new_commad_pool);
	}

	return new_commad_pool;
}

/*
*	Creates and returns a command pool from the "async compute" queue.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateAsyncComputeCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT new_commad_pool{ new (MemorySystem::Instance->TypeAllocate<VulkanCommandPool>()) VulkanCommandPool() };
	new_commad_pool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::ASYNC_COMPUTE));

	{
		SCOPED_LOCK(_VulkanCommandPoolsLock);

		_VulkanCommandPools.Emplace(new_commad_pool);
	}

	return new_commad_pool;
}

/*
*	Creates and returns a command pool from the "async transfer" queue.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateAsyncTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool* const RESTRICT new_commad_pool{ new (MemorySystem::Instance->TypeAllocate<VulkanCommandPool>()) VulkanCommandPool() };
	new_commad_pool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::ASYNC_TRANSFER));

	{
		SCOPED_LOCK(_VulkanCommandPoolsLock);

		_VulkanCommandPools.Emplace(new_commad_pool);
	}

	return new_commad_pool;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture *const RESTRICT VulkanInterface::CreateCubeMapTexture(const DynamicArray<DynamicArray<float32>> &data, const uint32 width, const uint32 height) NOEXCEPT
{
	VulkanCubeMapTexture *const RESTRICT new_cube_map_texture{ new (MemorySystem::Instance->TypeAllocate<VulkanCubeMapTexture>()) VulkanCubeMapTexture() };
	new_cube_map_texture->Initialize(data, width, height);

	{
		SCOPED_LOCK(_VulkanCubeMapTexturesLock);

		_VulkanCubeMapTextures.Emplace(new_cube_map_texture);
	}

	return new_cube_map_texture;
}

/*
*	Creates and returns a depth buffer.
*/
RESTRICTED VulkanDepthBuffer *const RESTRICT VulkanInterface::CreateDepthBuffer(const VkExtent2D &extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT
{
	VulkanDepthBuffer *const RESTRICT new_depth_buffer{ new (MemorySystem::Instance->TypeAllocate<VulkanDepthBuffer>()) VulkanDepthBuffer() };
	new_depth_buffer->Initialize(extent, format, sample_count);

	{
		SCOPED_LOCK(_VulkanDepthBuffersLock);

		_VulkanDepthBuffers.Emplace(new_depth_buffer);
	}

	return new_depth_buffer;
}

/*
*	Destroys a depth buffer.
*/
void VulkanInterface::DestroyDepthBuffer(VulkanDepthBuffer *const RESTRICT depth_buffer) NOEXCEPT
{
	depth_buffer->Release();

	{
		SCOPED_LOCK(_VulkanDepthBuffersLock);

		_VulkanDepthBuffers.Erase<false>(depth_buffer);
	}

	MemorySystem::Instance->TypeFree<VulkanDepthBuffer>(depth_buffer);
}

/*
*	Creates and returns a descriptor set layout.
*/
RESTRICTED VulkanDescriptorSetLayout *const RESTRICT VulkanInterface::CreateDescriptorSetLayout(const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings, const uint32 numberOfDescriptorSetLayoutBindings) NOEXCEPT
{
	VulkanDescriptorSetLayout *const RESTRICT new_descriptor_set_layout{ new (MemorySystem::Instance->TypeAllocate<VulkanDescriptorSetLayout>()) VulkanDescriptorSetLayout() };
	new_descriptor_set_layout->Initialize(numberOfDescriptorSetLayoutBindings, descriptorSetLayoutBindings);

	{
		SCOPED_LOCK(_VulkanDescriptorSetLayoutsLock);

		_VulkanDescriptorSetLayouts.Emplace(new_descriptor_set_layout);
	}

	return new_descriptor_set_layout;
}

/*
*	Destroys a descriptor set layout.
*/
void VulkanInterface::DestroyDescriptorSetLayout(VulkanDescriptorSetLayout *const RESTRICT descriptor_set_layout) NOEXCEPT
{
	descriptor_set_layout->Release();

	{
		SCOPED_LOCK(_VulkanDescriptorSetLayoutsLock);

		_VulkanDescriptorSetLayouts.Erase<false>(descriptor_set_layout);
	}

	MemorySystem::Instance->TypeFree<VulkanDescriptorSetLayout>(descriptor_set_layout);
}

/*
*	Creates and returns a descriptor set.
*/
RESTRICTED VulkanDescriptorSet *const RESTRICT VulkanInterface::CreateDescriptorSet(const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT
{
	VulkanDescriptorSet *const RESTRICT new_descriptor_set{ new (MemorySystem::Instance->TypeAllocate<VulkanDescriptorSet>()) VulkanDescriptorSet() };
	_VulkanDescriptorPool.AllocateDescriptorSet(*new_descriptor_set, vulkanDescriptorSetLayout);

	{
		SCOPED_LOCK(_VulkanDescriptorSetsLock);

		_VulkanDescriptorSets.Emplace(new_descriptor_set);
	}

	return new_descriptor_set;
}

/*
*	Destroys a descriptor set.
*/
void VulkanInterface::DestroyDescriptorSet(VulkanDescriptorSet *const RESTRICT descriptor_set) NOEXCEPT
{
	_VulkanDescriptorPool.FreeDescriptorSet(descriptor_set->Get());

	{
		SCOPED_LOCK(_VulkanDescriptorSetsLock);

		_VulkanDescriptorSets.Erase<false>(descriptor_set);
	}

	MemorySystem::Instance->TypeFree<VulkanDescriptorSet>(descriptor_set);
}

/*
*	Creates and returns an event.
*/
RESTRICTED VulkanEvent *const RESTRICT VulkanInterface::CreateEvent() NOEXCEPT
{
	VulkanEvent *const RESTRICT new_event{ new (MemorySystem::Instance->TypeAllocate<VulkanEvent>()) VulkanEvent() };
	new_event->Initialize();

	{
		SCOPED_LOCK(_VulkanEventsLock);

		_VulkanEvents.Emplace(new_event);
	}

	return new_event;
}

/*
*	Creates and returns a fence.
*/
RESTRICTED VulkanFence *const RESTRICT VulkanInterface::CreateFence(const VkFenceCreateFlags flags) NOEXCEPT
{
	VulkanFence *const RESTRICT new_fence{ new (MemorySystem::Instance->TypeAllocate<VulkanFence>()) VulkanFence() };
	new_fence->Initialize(flags);

	{
		SCOPED_LOCK(_VulkanFencesLock);

		_VulkanFences.Emplace(new_fence);
	}

	return new_fence;
}

/*
*	Creates and returns a framebuffer.
*/
RESTRICTED VulkanFramebuffer *const RESTRICT VulkanInterface::CreateFramebuffer(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT
{
	VulkanFramebuffer *const RESTRICT new_framebuffer{ new (MemorySystem::Instance->TypeAllocate<VulkanFramebuffer>()) VulkanFramebuffer() };
	new_framebuffer->Initialize(parameters);

	{
		SCOPED_LOCK(_VulkanFramebuffersLock);

		_VulkanFramebuffers.Emplace(new_framebuffer);
	}

	return new_framebuffer;
}

/*
*	Destroys a framebuffer.
*/
void VulkanInterface::DestroyFramebuffer(VulkanFramebuffer *const RESTRICT framebuffer) NOEXCEPT
{
	framebuffer->Release();
	
	{
		SCOPED_LOCK(_VulkanFramebuffersLock);

		_VulkanFramebuffers.Erase<false>(framebuffer);
	}

	MemorySystem::Instance->TypeFree<VulkanFramebuffer>(framebuffer);
}

/*
*	Creates and returns a compute pipeline.
*/
RESTRICTED VulkanComputePipeline *const RESTRICT VulkanInterface::CreateComputePipeline(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanComputePipeline *const RESTRICT new_pipeline{ new (MemorySystem::Instance->TypeAllocate<VulkanComputePipeline>()) VulkanComputePipeline() };
	new_pipeline->Initialize(parameters);

	{
		SCOPED_LOCK(_VulkanComputePipelinesLock);

		_VulkanComputePipelines.Emplace(new_pipeline);
	}

	return new_pipeline;
}

/*
*	Destroys a compute pipeline.
*/
void VulkanInterface::DestroyComputePipeline(VulkanComputePipeline *const RESTRICT pipeline) NOEXCEPT
{
	pipeline->Release();
	
	{
		SCOPED_LOCK(_VulkanComputePipelinesLock);

		_VulkanComputePipelines.Erase<false>(pipeline);
	}

	MemorySystem::Instance->TypeFree<VulkanComputePipeline>(pipeline);
}

/*
*	Creates and returns a graphics pipeline.
*/
RESTRICTED VulkanGraphicsPipeline *const RESTRICT VulkanInterface::CreateGraphicsPipeline(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanGraphicsPipeline *const RESTRICT new_pipeline{ new (MemorySystem::Instance->TypeAllocate<VulkanGraphicsPipeline>()) VulkanGraphicsPipeline() };
	new_pipeline->Initialize(parameters);

	{
		SCOPED_LOCK(_VulkanGraphicsPipelinesLock);

		_VulkanGraphicsPipelines.Emplace(new_pipeline);
	}

	return new_pipeline;
}

/*
*	Destroys a graphics pipeline.
*/
void VulkanInterface::DestroyGraphicsPipeline(VulkanGraphicsPipeline *const RESTRICT pipeline) NOEXCEPT
{
	pipeline->Release();
	
	{
		SCOPED_LOCK(_VulkanGraphicsPipelinesLock);

		_VulkanGraphicsPipelines.Erase<false>(pipeline);
	}

	MemorySystem::Instance->TypeFree<VulkanGraphicsPipeline>(pipeline);
}

/*
*	Creates and returns a ray tracing pipeline.
*/
RESTRICTED VulkanRayTracingPipeline *const RESTRICT VulkanInterface::CreateRayTracingPipeline(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanRayTracingPipeline *const RESTRICT new_pipeline{ new (MemorySystem::Instance->TypeAllocate<VulkanRayTracingPipeline>()) VulkanRayTracingPipeline() };
	new_pipeline->Initialize(parameters);

	{
		SCOPED_LOCK(_VulkanRayTracingPipelinesLock);

		_VulkanRayTracingPipelines.Emplace(new_pipeline);
	}

	return new_pipeline;
}

/*
*	Destroys a ray tracing pipeline.
*/
void VulkanInterface::DestroyRayTracingPipeline(VulkanRayTracingPipeline *const RESTRICT pipeline) NOEXCEPT
{
	pipeline->Release();
	
	{
		SCOPED_LOCK(_VulkanRayTracingPipelinesLock);

		_VulkanRayTracingPipelines.Erase<false>(pipeline);
	}

	MemorySystem::Instance->TypeFree<VulkanRayTracingPipeline>(pipeline);
}

/*
*	Creates and returns a query pool.
*/
RESTRICTED VulkanQueryPool *const RESTRICT VulkanInterface::CreateQueryPool(const VkQueryType query_type, const uint32 query_count) NOEXCEPT
{
	VulkanQueryPool *const RESTRICT new_query_pool{ new (MemorySystem::Instance->TypeAllocate<VulkanQueryPool>()) VulkanQueryPool() };
	new_query_pool->Initialize(query_type, query_count);

	{
		SCOPED_LOCK(_VulkanQueryPoolsLock);

		_VulkanQueryPools.Emplace(new_query_pool);
	}

	return new_query_pool;
}

/*
*	Creates and returns a render pass.
*/
RESTRICTED VulkanRenderPass *const RESTRICT VulkanInterface::CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT
{
	VulkanRenderPass *const RESTRICT new_render_pass{ new (MemorySystem::Instance->TypeAllocate<VulkanRenderPass>()) VulkanRenderPass() };
	new_render_pass->Initialize(parameters);

	{
		SCOPED_LOCK(_VulkanRenderPassesLock);

		_VulkanRenderPasses.Emplace(new_render_pass);
	}

	return new_render_pass;
}

/*
*	Destroys a render pass.
*/
void VulkanInterface::DestroyRenderPass(VulkanRenderPass *const RESTRICT render_pass) NOEXCEPT
{
	render_pass->Release();
	
	{
		SCOPED_LOCK(_VulkanRenderPassesLock);

		_VulkanRenderPasses.Erase<false>(render_pass);
	}

	MemorySystem::Instance->TypeFree<VulkanRenderPass>(render_pass);
}

/*
*	Creates and returns a render target.
*/
RESTRICTED VulkanRenderTarget *const RESTRICT VulkanInterface::CreateRenderTarget(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT
{
	VulkanRenderTarget *const RESTRICT new_render_target{ new (MemorySystem::Instance->TypeAllocate<VulkanRenderTarget>()) VulkanRenderTarget() };
	new_render_target->Initialize(extent, format, sample_count);

	{
		SCOPED_LOCK(_VulkanRenderTargetsLock);

		_VulkanRenderTargets.Emplace(new_render_target);
	}

	return new_render_target;
}

/*
*	Destroys a render target.
*/
void VulkanInterface::DestroyRenderTarget(VulkanRenderTarget *const RESTRICT render_target) NOEXCEPT
{
	render_target->Release();
	
	{
		SCOPED_LOCK(_VulkanRenderTargetsLock);

		_VulkanRenderTargets.Erase<false>(render_target);
	}

	MemorySystem::Instance->TypeFree<VulkanRenderTarget>(render_target);
}

/*
*	Creates and returns a semaphore.
*/
RESTRICTED VulkanSemaphore *const RESTRICT VulkanInterface::CreateSemaphore() NOEXCEPT
{
	VulkanSemaphore *const RESTRICT new_semaphore{ new (MemorySystem::Instance->TypeAllocate<VulkanSemaphore>()) VulkanSemaphore() };
	new_semaphore->Initialize();

	{
		SCOPED_LOCK(_VulkanSemaphoresLock);

		_VulkanSemaphores.Emplace(new_semaphore);
	}

	return new_semaphore;
}

/*
*	Creates and returns a sampler.
*/
RESTRICTED VulkanSampler *const RESTRICT VulkanInterface::CreateSampler(const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) NOEXCEPT
{
	VulkanSampler *const RESTRICT new_sampler{ new (MemorySystem::Instance->TypeAllocate<VulkanSampler>()) VulkanSampler() };
	new_sampler->Initialize(magnification_filter, mipmap_mode, address_mode, anisotropic_samples);

	{
		SCOPED_LOCK(_VulkanSamplersLock);

		_VulkanSamplers.Emplace(new_sampler);
	}

	return new_sampler;
}

/*
*	Creates and returns a shader module.
*/
RESTRICTED VulkanShaderModule *const RESTRICT VulkanInterface::CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT
{
	VulkanShaderModule *const RESTRICT new_shader_module{ new (MemorySystem::Instance->TypeAllocate<VulkanShaderModule>()) VulkanShaderModule() };
	new_shader_module->Initialize(shaderData, shaderDataSize, stage);

	{
		SCOPED_LOCK(_VulkanShaderModulesLock);

		_VulkanShaderModules.Emplace(new_shader_module);
	}

	return new_shader_module;
}

/*
*	Create the Vulkan memory allocator.
*/
void VulkanInterface::CreateVulkanMemoryAllocator() NOEXCEPT
{
	VmaAllocatorCreateInfo info;

	info.flags = 0;
	info.physicalDevice = _VulkanPhysicalDevice.Get();
	info.device = _VulkanLogicalDevice.Get();
	info.preferredLargeHeapBlockSize = 0;
	info.pAllocationCallbacks = nullptr;
	info.pDeviceMemoryCallbacks = nullptr;
	info.pHeapSizeLimit = nullptr;
	info.pVulkanFunctions = nullptr;
	info.instance = _VulkanInstance.Get();

#if defined(CATALYST_PLATFORM_ANDROID)
	info.vulkanApiVersion = VK_API_VERSION_1_0;
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	info.vulkanApiVersion = VK_API_VERSION_1_1;
#endif

	info.pTypeExternalMemoryHandleTypes = nullptr;

	vmaCreateAllocator(&info, &VULKAN_MEMORY_ALLOCATOR);
}

/*
*	Destroys the Vulkan memory allocator.
*/
void VulkanInterface::DestroyVulkanMemoryAllocator() NOEXCEPT
{
	vmaDestroyAllocator(VULKAN_MEMORY_ALLOCATOR);
}
#endif