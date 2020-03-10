#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TerrainRayTracingData final
{

public:

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	//The vertex buffers.
	StaticArray<BufferHandle, 2> _VertexBuffers{ EMPTY_HANDLE };

	//The index buffers.
	StaticArray<BufferHandle, 2> _IndexBuffers{ EMPTY_HANDLE };

	//The bottom level acceleration structures.
	StaticArray<AccelerationStructureHandle, 2> _BottomLevelAccelerationStructures{ EMPTY_HANDLE };

	//The top level acceleration structures.
	StaticArray<AccelerationStructureHandle, 2> _TopLevelAccelerationStructures{ EMPTY_HANDLE };

};