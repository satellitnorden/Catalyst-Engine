#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/RenderingCore.h>

class PhysicalModel final
{

public:

	//The model space axis-aligned bounding box of this physical model.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset in the buffer.
	uint64 _IndexOffset;

	//The number of indices that this physical model has.
	uint32 _IndexCount;

	/*
	*	Default constructor.
	*/
	PhysicalModel() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	PhysicalModel(const PhysicalModel &otherPhysicalModel) NOEXCEPT
		:
		_AxisAlignedBoundingBox(otherPhysicalModel._AxisAlignedBoundingBox),
		_Buffer(otherPhysicalModel._Buffer),
		_IndexOffset(otherPhysicalModel._IndexOffset),
		_IndexCount(otherPhysicalModel._IndexCount)
	{

	}

};