#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/WorldSystem.h>

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
	*	Constructor taking the local position.
	*/
	FORCE_INLINE explicit WorldPosition(	const Vector3<float32> &initial_local_position) NOEXCEPT
		:
		_Cell(0, 0, 0),
		_LocalPosition(initial_local_position)
	{
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
	FORCE_INLINE NO_DISCARD Vector3<float32> GetAbsolutePosition() const NOEXCEPT
	{	
		const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

		return _LocalPosition + Vector3<float32>(static_cast<float32>(_Cell._X), static_cast<float32>(_Cell._Y), static_cast<float32>(_Cell._Z)) * world_grid_size;
	}

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
	FORCE_INLINE NO_DISCARD Vector3<float32> GetRelativePosition(const Vector3<int32> &cell) const NOEXCEPT
	{	
		const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

		const Vector3<int32> delta{ _Cell - cell };

		return _LocalPosition + Vector3<float32>(static_cast<float32>(delta._X), static_cast<float32>(delta._Y), static_cast<float32>(delta._Z)) * world_grid_size;
	}

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
	FORCE_INLINE void UpdateCell() NOEXCEPT
	{
		const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };
		const float32 half_world_grid_size{ world_grid_size * 0.5f };

		while (_LocalPosition._X < -half_world_grid_size)
		{
			_LocalPosition._X += world_grid_size;
			--_Cell._X;
		}

		while (_LocalPosition._X > half_world_grid_size)
		{
			_LocalPosition._X -= world_grid_size;
			++_Cell._X;
		}

		while (_LocalPosition._Y < -half_world_grid_size)
		{
			_LocalPosition._Y += world_grid_size;
			--_Cell._Y;
		}

		while (_LocalPosition._Y > half_world_grid_size)
		{
			_LocalPosition._Y -= world_grid_size;
			++_Cell._Y;
		}

		while (_LocalPosition._Z < -half_world_grid_size)
		{
			_LocalPosition._Z += world_grid_size;
			--_Cell._Z;
		}

		while (_LocalPosition._Z > half_world_grid_size)
		{
			_LocalPosition._Z -= world_grid_size;
			++_Cell._Z;
		}
	}

};