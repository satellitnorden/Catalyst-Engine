#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class Mesh final
{

public:

	//The vertex buffers for all level of details.
	DynamicArray<BufferHandle> _VertexBuffers;

	//The index buffers for all level of details.
	DynamicArray<BufferHandle> _IndexBuffers;

	//The index counts for all level of details.
	DynamicArray<uint32> _IndexCounts;

	//The bottom level acceleration structure.
	AccelerationStructureHandle _BottomLevelAccelerationStructure;

};