#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class GrassModel final
{

public:

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset in the buffer.
	uint64 _IndexOffset;

	//The number of indices that this grass model has.
	uint32 _IndexCount;

	/*
	*	Default constructor.
	*/
	GrassModel() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	GrassModel(const GrassModel &other) NOEXCEPT
		:
		_Buffer(other._Buffer),
		_IndexOffset(other._IndexOffset),
		_IndexCount(other._IndexCount)
	{

	}

};