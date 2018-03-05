#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <TextureData.h>

//Vulkan.
#include <VulkanCore.h>

namespace VulkanTranslationUtilities
{
	/*
	*	Given an address mode, return the corresponding Vulkan address mode.
	*/
	static VkSamplerAddressMode GetVulkanAddressMode(const AddressMode addressMode) NOEXCEPT
	{
		switch (addressMode)
		{
			case AddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case AddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case AddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			case AddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case AddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;

			default:
			{
#if !defined(CATALYST_FINAL)
				PRINT_TO_CONSOLE("Unknown address mode."); BREAKPOINT;
#endif
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
		}
	}

	/*
	*	Given a format, return the corresponding Vulkan format.
	*/
	static VkFormat GetVulkanFormat(const TextureFormat format) NOEXCEPT
	{
		switch (format)
		{
			case TextureFormat::R8_Byte: return VK_FORMAT_R8_UNORM;
			case TextureFormat::R8G8B8A8_Byte: return VK_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::R32G32B32A32_Float: return VK_FORMAT_R32G32B32A32_SFLOAT;

			default:
			{
#if !defined(CATALYST_FINAL)
				PRINT_TO_CONSOLE("Unknown texture format."); BREAKPOINT;
#endif
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
		}
	}

	/*
	*	Given a mipmap mode, return the corresponding Vulkan mipmap mode.
	*/
	static VkSamplerMipmapMode GetVulkanMipmapMode(const MipmapMode mipmapMode) NOEXCEPT
	{
		switch (mipmapMode)
		{
			case MipmapMode::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			case MipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			default:
			{
#if !defined(CATALYST_FINAL)
				PRINT_TO_CONSOLE("Unknown mipmap mode."); BREAKPOINT;
#endif
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			}
		}
	}

	/*
	*	Given a texture filter, return the corresponding Vulkan texture filter.
	*/
	static VkFilter GetVulkanTextureFilter(const TextureFilter textureFilter) NOEXCEPT
	{
		switch (textureFilter)
		{
			case TextureFilter::Linear: return VK_FILTER_LINEAR;
			case TextureFilter::Nearest: return VK_FILTER_NEAREST;

			default:
			{
#if !defined(CATALYST_FINAL)
				PRINT_TO_CONSOLE("Unknown texture filter."); BREAKPOINT;
#endif
				return VK_FILTER_NEAREST;
			}
		}
	}

	/*
	*	Returns the default vertex input binding description.
	*/
	static void GetDefaultVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = 0;
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	/*
	*	Returns the vertex input attribute descriptions for the physical entity scene buffer render pass.
	*/
	static void GetPhysicalVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 4> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = offsetof(PhysicalVertex, position);

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = offsetof(PhysicalVertex, normal);

		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[2].offset = offsetof(PhysicalVertex, tangent);

		vertexInputAttributeDescriptions[3].location = 3;
		vertexInputAttributeDescriptions[3].binding = 0;
		vertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[3].offset = offsetof(PhysicalVertex, textureCoordinate);
	}

	/*
	*	Returns the vertex input binding description for the physical entity scene buffer render pass.
	*/
	static void GetPhysicalVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(PhysicalVertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	/*
	*	Returns the vertex input attribute descriptions for the terrain entity scene buffer render pass.
	*/
	static void GetTerrainVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 2> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = 0;

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;
	}

	/*
	*	Returns the vertex input binding description for the terrain entity scene buffer render pass.
	*/
	static void GetTerrainVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(float) * 5;
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}
}