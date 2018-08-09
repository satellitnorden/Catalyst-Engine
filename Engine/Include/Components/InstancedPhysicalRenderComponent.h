#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class InstancedPhysicalRenderComponent final
{

public:

	//The descriptor set.
	RenderDataTableHandle renderDataTable;

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