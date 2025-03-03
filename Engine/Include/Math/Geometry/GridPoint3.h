#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>
#include <Math/General/Vector.h>

class GridPoint3 final
{

public:

	//The X position.
	int32 _X;

	//The Y position.
	int32 _Y;

	//The Z position.
	int32 _Z;

	/*
	*	Default constructor.
	*/
	GridPoint3() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	GridPoint3(const int32 initialX, const int32 initialY, const int32 initialZ) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY),
		_Z(initialZ)
	{

	}


	/*
	*	Equality operator overload.
	*/
	constexpr bool operator==(const GridPoint3 &other) const NOEXCEPT
	{
		return _X == other._X && _Y == other._Y && _Z == other._Z;
	}

	/*
	*	Inequality operator overload.
	*/
	constexpr bool operator!=(const GridPoint3 &other) const NOEXCEPT
	{
		return !(operator==(other));
	}

	/*
	*	Calculates the world position from a grid point.
	*/
	static NO_DISCARD Vector3<float> GridPointToWorldPosition(const GridPoint3 &gridPoint, const float gridSize) NOEXCEPT
	{
		return Vector3<float>(	gridPoint._X * gridSize,
								gridPoint._Y * gridSize,
								gridPoint._Z * gridSize);
	}

	/*
	*	Calculates the grid point from a world position.
	*/
	static NO_DISCARD GridPoint3 WorldPositionToGridPoint(const Vector3<float> &worldPosition, const float gridSize) NOEXCEPT
	{
		return GridPoint3(	BaseMath::Round<int32>(worldPosition._X / gridSize),
							BaseMath::Round<int32>(worldPosition._Y / gridSize),
							BaseMath::Round<int32>(worldPosition._Z / gridSize));
	}

};