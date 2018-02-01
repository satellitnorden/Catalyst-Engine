#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanUniformBuffer.h>

class GraphicsBufferComponent
{

public:

	//The uniform buffer.
	VulkanUniformBuffer uniformBuffer{ };

};