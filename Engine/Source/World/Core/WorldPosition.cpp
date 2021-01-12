//Header file.
#include <World/Core/WorldPosition.h>

//Systems.
#include <Systems/WorldSystem.h>

/*
*	Returns the absolute position.
*/
NO_DISCARD Vector3<float32> WorldPosition::GetAbsolutePosition() const NOEXCEPT
{	
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

	return _LocalPosition + Vector3<float32>(static_cast<float32>(_Cell._X), static_cast<float32>(_Cell._Y), static_cast<float32>(_Cell._Z)) * world_grid_size;
}

/*
*	Returns the relative position.
*/
NO_DISCARD Vector3<float32> WorldPosition::GetRelativePosition(const Vector3<int32> &cell) const NOEXCEPT
{	
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

	const Vector3<int32> delta{ _Cell - cell };

	return _LocalPosition + Vector3<float32>(static_cast<float32>(delta._X), static_cast<float32>(delta._Y), static_cast<float32>(delta._Z)) * world_grid_size;
}

/*
*	Updates the cell.
*/
void WorldPosition::UpdateCell() NOEXCEPT
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