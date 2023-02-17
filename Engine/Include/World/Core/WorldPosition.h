#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class WorldPosition final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE explicit WorldPosition() NOEXCEPT
		:
		_Cell(0, 0, 0),
		_LocalPosition(0.0f, 0.0f, 0.0f)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE explicit WorldPosition(const Vector3<int32> &initial_cell,
										const Vector3<float32> &initial_local_position) NOEXCEPT
		:
		_Cell(initial_cell),
		_LocalPosition(initial_local_position)
	{
		//Update the cell.
		UpdateCell();
	}

	/*
	*	Constructor taking the cell.
	*/
	FORCE_INLINE explicit WorldPosition(const Vector3<int32> &initial_cell) NOEXCEPT
		:
		_Cell(initial_cell),
		_LocalPosition(0.0f, 0.0f, 0.0f)
	{
		
	}

	/*
	*	Constructor taking the local position.
	*/
	FORCE_INLINE explicit WorldPosition(const Vector3<float32> &initial_local_position) NOEXCEPT
		:
		_Cell(0, 0, 0),
		_LocalPosition(initial_local_position)
	{
		//Update the cell.
		UpdateCell();
	}

	/*
	*	Vector3 addition assignment operator overload.
	*/
	FORCE_INLINE void operator+=(const Vector3<float32> &vector) NOEXCEPT
	{
		//Add to the local position.
		_LocalPosition += vector;

		//Update the cell.
		UpdateCell();
	}

	/*
	*	Returns the cell.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<int32> &GetCell() const NOEXCEPT
	{
		return _Cell;
	}

	/*
	*	Sets the cell.
	*/
	FORCE_INLINE void SetCell(const Vector3<int32> &value) NOEXCEPT
	{
		_Cell = value;
	}

	/*
	*	Returns the local position.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<float32> &GetLocalPosition() const NOEXCEPT
	{
		return _LocalPosition;
	}

	/*
	*	Sets the local position.
	*/
	FORCE_INLINE void SetLocalPosition(const Vector3<float32> &value) NOEXCEPT
	{
		_LocalPosition = value;

		UpdateCell();
	}

	/*
	*	Returns the absolute position.
	*/
	NO_DISCARD Vector3<float32> GetAbsolutePosition() const NOEXCEPT;

	/*
	*	Sets the absolute position.
	*/
	FORCE_INLINE void SetAbsolutePosition(const Vector3<float32>& value) NOEXCEPT
	{
		_Cell = Vector3<int32>(0, 0, 0);
		_LocalPosition = value;

		UpdateCell();
	}

	/*
	*	Returns the relative position.
	*/
	NO_DISCARD Vector3<float32> GetRelativePosition(const Vector3<int32>& cell) const NOEXCEPT;

	/*
	*	Sets the relative position.
	*/
	FORCE_INLINE void SetRelativePosition(const Vector3<int32> &cell, const Vector3<float32> &value) NOEXCEPT
	{
		_Cell = cell;
		_LocalPosition = value;

		UpdateCell();
	}

private:

	//The cell.
	Vector3<int32> _Cell;

	//The local position.
	Vector3<float32> _LocalPosition;

	/*
	*	Updates the cell.
	*/
	void UpdateCell() NOEXCEPT;

};