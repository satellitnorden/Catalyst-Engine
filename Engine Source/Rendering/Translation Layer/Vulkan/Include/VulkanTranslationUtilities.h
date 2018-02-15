#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
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
}