#if defined(CATALYST_WINDOWS)

#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <ThirdParty/vulkan/vulkan.h>

namespace PlatformVulkan
{

	/*
	*	Loads Vulkan.
	*/
	static void LoadVulkan() NOEXCEPT
	{
		//Nothing to do here on Windows.
	}

}

#endif