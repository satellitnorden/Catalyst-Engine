#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

class GridPoint2 final
{

public:

	//The X position.
	int32 _X{ 0 };

	//The Y position.
	int32 _Y{ 0 };

	/*
	*	Default constructor.
	*/
	constexpr GridPoint2() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr GridPoint2(const int32 initialX, const int32 initialY) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY)
	{

	}


	/*
	*	Equality operator overload.
	*/
	constexpr bool operator==(const GridPoint2 &other) const NOEXCEPT
	{
		return _X == other._X && _Y == other._Y;
	}
		
	/*
	*	Inequality operator overload.
	*/
	constexpr bool operator!=(const GridPoint2 &other) const NOEXCEPT
	{
		return !(operator==(other));
	}

	/*
	*	Multiplication by scalar operator overload.
	*/
	constexpr GridPoint2 operator*(const int32 scalar) const NOEXCEPT
	{
		return GridPoint2(_X * scalar, _Y * scalar);
	}

	/*
	*	Calculates the world position from a grid point.
	*/
	constexpr static NO_DISCARD Vector3 GridPointToWorldPosition(const GridPoint2 &gridPoint, const float gridSize) NOEXCEPT
	{
		return Vector3(	gridPoint._X * gridSize , 0.0f,
						gridPoint._Y * gridSize);
	}

	/*
	*	Calculates the grid point from a world position.
	*/
	constexpr static NO_DISCARD GridPoint2 WorldPositionToGridPoint(const Vector3 &worldPosition, const float gridSize) NOEXCEPT
	{
		return GridPoint2(	CatalystBaseMath::Round<int32>(worldPosition._X / gridSize),
							CatalystBaseMath::Round<int32>(worldPosition._Z / gridSize));
	}

};