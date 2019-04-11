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

	//The axis-aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The vertices of this model.
	DynamicArray<Vertex> _Vertices;

	//The indices of this model.
	DynamicArray<uint32> _Indices;

	//The vertex buffer.
	BufferHandle _VertexBuffer;

	//The index buffer.
	BufferHandle _IndexBuffer;

	//The acceleration structure.
	AccelerationStructureHandle _AccelerationStructure;

};