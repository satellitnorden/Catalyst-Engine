#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Vertex.h>

class Model final
{

public:

	//The model space axis-aligned bounding box.
	AxisAlignedBoundingBox _ModelSpaceAxisAlignedBoundingBox;

	//The vertex buffers.
	DynamicArray<BufferHandle> _VertexBuffers;

	//The index buffers.
	DynamicArray<BufferHandle> _IndexBuffers;

	//The index counts.
	DynamicArray<uint32> _IndexCounts;

	//The bottom level acceleration structure.
	AccelerationStructureHandle _BottomLevelAccelerationStructure;

};