#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

class InstancedPhysicalRenderComponent final
{

public:

	//The descriptor set.
	DescriptorSetHandle descriptorSet;

	//The model buffer.
	GraphicsBufferHandle modelBuffer;

	//The transformations buffer.
	GraphicsBufferHandle transformationsBuffer;

	//The offset for the indices in the buffer.
	uint64 indexOffset;

	//The index count.
	uint32 indexCount;

	//The number of instances.
	uint32 instanceCount;

};