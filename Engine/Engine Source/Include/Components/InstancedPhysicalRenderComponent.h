#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class InstancedPhysicalRenderComponent final
{

public:

	//The descriptor set.
	DescriptorSetHandle descriptorSet;

	//The model buffer.
	ConstantBufferHandle modelBuffer;

	//The transformations buffer.
	ConstantBufferHandle transformationsBuffer;

	//The offset for the indices in the buffer.
	uint64 indexOffset;

	//The index count.
	uint32 indexCount;

	//The number of instances.
	uint32 instanceCount;

};