#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/Skeleton.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class AnimatedModel final
{

public:

	//The model space axis-aligned bounding box.
	AxisAlignedBoundingBox _ModelSpaceAxisAlignedBoundingBox;

	//The vertex buffer.
	BufferHandle _VertexBuffer;

	//The index buffer.
	BufferHandle _IndexBuffer;

	//The index count.
	uint32 _IndexCount;

	//The bottom level acceleration structure.
	AccelerationStructureHandle _BottomLevelAccelerationStructure;

	//The skeleton.
	Skeleton _Skeleton;

};