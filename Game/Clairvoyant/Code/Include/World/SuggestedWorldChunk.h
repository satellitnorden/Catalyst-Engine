#pragma once

//Core.
#include <Core/EngineCore.h>

class SuggestedWorldChunk final
{

public:

	//The X grid position.
	int32 gridPositionX{ INT32_MAXIMUM };

	//The Y grid position.
	int32 gridPositionY{ INT32_MAXIMUM };

	/*
	*	Default constructor.
	*/
	SuggestedWorldChunk() NOEXCEPT = default;

	/*
	*	Constructor taking all values as arguments.
	*/
	SuggestedWorldChunk(const int32 initialGridPositionX, const int32 initialGridPositionY) NOEXCEPT
		:
		gridPositionX(initialGridPositionX),
		gridPositionY(initialGridPositionY)
	{

	}

	/*
	*	Returns whether or not this suggested world chunk is valid.
	*/
	bool IsValid() const NOEXCEPT { return gridPositionX != INT32_MAXIMUM && gridPositionY != INT32_MAXIMUM; }

	/*
	*	Invalidates this suggested world chunk.
	*/
	void Invalidate() NOEXCEPT
	{
		gridPositionX = INT32_MAXIMUM;
		gridPositionY = INT32_MAXIMUM;
	}

};