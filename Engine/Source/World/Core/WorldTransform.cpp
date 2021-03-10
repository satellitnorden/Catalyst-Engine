//Header file.
#include <World/Core/WorldTransform.h>

//Systems.
#include <Systems/WorldSystem.h>

/*
*	Converts this world transform into an absolute Matrix4x4.
*/
NO_DISCARD Matrix4x4 WorldTransform::ToAbsoluteMatrix4x4() const NOEXCEPT
{
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

	return Matrix4x4(_WorldPosition.GetLocalPosition() + Vector3<float32>(static_cast<float32>(_WorldPosition.GetCell()._X), static_cast<float32>(_WorldPosition.GetCell()._Y), static_cast<float32>(_WorldPosition.GetCell()._Z)) * world_grid_size, _Rotation, Vector3<float32>(_Scale));
}

/*
*	Converts this world transform into a relative Matrix4x4 as seen from the given cell.
*/
NO_DISCARD Matrix4x4 WorldTransform::ToRelativeMatrix4x4(const Vector3<int32> &cell) const NOEXCEPT
{
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

	const Vector3<int32> delta{ _WorldPosition.GetCell() - cell };

	return Matrix4x4(_WorldPosition.GetLocalPosition() + Vector3<float32>(static_cast<float32>(delta._X), static_cast<float32>(delta._Y), static_cast<float32>(delta._Z)) * world_grid_size, _Rotation, Vector3<float32>(_Scale));
}