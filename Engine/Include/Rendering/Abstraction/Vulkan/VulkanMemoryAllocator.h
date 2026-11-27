#if defined(CATALYST_HAS_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <Vulkan/Volk/volk.h>
#define VMA_VULKAN_HEADERS_ALREADY_INCLUDED
#include <vma/vk_mem_alloc.h>

//The Vulkan memory allocator.
extern VmaAllocator VULKAN_MEMORY_ALLOCATOR;
#endif