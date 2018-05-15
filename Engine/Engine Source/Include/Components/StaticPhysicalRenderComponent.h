#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Matrix4.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanConstantBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>

class StaticPhysicalRenderComponent final
{

public:

	//The model matrix.
	Matrix4 modelMatrix;

	//The descriptor set.
	DescriptorSetHandle descriptorSet{ };

	//The buffer.
	VulkanConstantBuffer buffer{ };

	//The offset for the indices in the buffer.
	uint64 indexOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};