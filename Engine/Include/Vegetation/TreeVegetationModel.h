#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/RenderingCore.h>

class TreeVegetationModel final
{

public:

	//The model space axis-aligned bounding box of this tree vegetation model.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset in the buffer.
	uint64 _IndexOffset;

	//The number of indices that this grtree vegetationass model has.
	uint32 _IndexCount;

	/*
	*	Default constructor.
	*/
	TreeVegetationModel() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	TreeVegetationModel(const TreeVegetationModel &other) NOEXCEPT
		:
		_Buffer(other._Buffer),
		_IndexOffset(other._IndexOffset),
		_IndexCount(other._IndexCount)
	{

	}

};