#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

class GridPoint final
{

public:

	//The X position.
	int32 _X;

	//The Y position.
	int32 _Y;

	/*
	*	Default constructor.
	*/
	GridPoint() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	GridPoint(const int32 initialX, const int32 initialY) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY)
	{

	}


	/*
	*	Equality operator overload.
	*/
	constexpr bool operator==(const GridPoint &other) const NOEXCEPT
	{
		return _X == other._X && _Y == other._Y;
	}
		
	/*
	*	Inequality operator overload.
	*/
	constexpr bool operator!=(const GridPoint &other) const NOEXCEPT
	{
		return !(operator==(other));
	}

	/*
	*	Calculates the world position from a grid point.
	*/
	static NO_DISCARD Vector3 GridPointToWorldPosition(const GridPoint &gridPoint, const float gridSize) NOEXCEPT
	{
		return Vector3(	gridPoint._X * gridSize , 0.0f,
						gridPoint._Y * gridSize);
	}

	/*
	*	Calculates the grid point from a world position.
	*/
	static NO_DISCARD GridPoint WorldPositionToGridPoint(const Vector3 &worldPosition, const float gridSize) NOEXCEPT
	{
		return GridPoint(	CatalystBaseMath::Round<int32>(worldPosition._X / gridSize),
							CatalystBaseMath::Round<int32>(worldPosition._Z / gridSize));
	}

};