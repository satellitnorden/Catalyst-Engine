#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/RenderingCore.h>

class GrassVegetationModel final
{

public:

	//The model space axis-aligned bounding box of this physical model.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset in the buffer.
	uint64 _IndexOffset;

	//The number of indices that this grass model has.
	uint32 _IndexCount;

	/*
	*	Default constructor.
	*/
	GrassVegetationModel() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	GrassVegetationModel(const GrassVegetationModel &other) NOEXCEPT
		:
		_Buffer(other._Buffer),
		_IndexOffset(other._IndexOffset),
		_IndexCount(other._IndexCount)
	{

	}

};