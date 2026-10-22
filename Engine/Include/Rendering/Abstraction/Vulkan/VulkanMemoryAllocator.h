#if defined(CATALYST_HAS_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <vma/vk_mem_alloc.h>

//The Vulkan memory allocator.
extern VmaAllocator VULKAN_MEMORY_ALLOCATOR;
#endif