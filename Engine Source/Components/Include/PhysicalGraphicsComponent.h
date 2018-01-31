#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanDescriptorSet.h>
#include <VulkanIndexBuffer.h>
#include <VulkanVertexBuffer.h>

class PhysicalGraphicsComponent
{

public:

	//Denotes whether or not the physical entity is in the view frustum.
	std::atomic<bool> isInViewFrustum{ true };

	//The descriptor set.
	VulkanDescriptorSet descriptorSet;

	//The vertex buffer.
	VulkanVertexBuffer vertexBuffer;

	//The index buffer.
	VulkanIndexBuffer indexBuffer;

	//The index count.
	uint32 indexCount;

};