#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationModel final
{

public:

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset in the buffer.
	uint64 _IndexOffset;

	//The number of indices that this vegetation model has.
	uint32 _IndexCount;

	/*
	*	Default constructor.
	*/
	VegetationModel() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	VegetationModel(const VegetationModel &otherPhysicalModel) NOEXCEPT
		:
		_Buffer(otherPhysicalModel._Buffer),
		_IndexOffset(otherPhysicalModel._IndexOffset),
		_IndexCount(otherPhysicalModel._IndexCount)
	{

	}

};