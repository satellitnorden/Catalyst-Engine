#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

//Systems.
#include <Systems/WorldSystem.h>

/*
*	In the Catalyst Engine™, to enable infinite worlds without floating point precision issues,
*	the world is divided into a three dimensional grid, where each grid cell has it's own space.
*	This is used for example in rendering where the final transforms of objects are specified relative
*	to the Perceiver's current grid cell.
*/
class WorldTransform final
{

public:

	//The cell.
	Vector3<int32> _Cell;

	//The position.
	Vector3<float32> _Position;

	//The rotation.
	Vector3<float32> _Rotation;

	//The scale.
	float32 _Scale;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr WorldTransform() NOEXCEPT
		:
		_Cell(),
		_Position(),
		_Rotation(),
		_Scale()
	{

	}

	/*
	*	Converts this world transform into an absolute Matrix4x4.
	*/
	FORCE_INLINE NO_DISCARD Matrix4x4 ToAbsoluteMatrix4x4() const NOEXCEPT
	{
		return Matrix4x4(_Position + Vector3<float32>(static_cast<float32>(_Cell._X), static_cast<float32>(_Cell._Y), static_cast<float32>(_Cell._Z)) * WorldSystem::Instance->GetWorldGridSize(), _Rotation, Vector3<float32>(_Scale));
	}

	/*
	*	Converts this world transform into a relative Matrix4x4 as seen from the given cell.
	*/
	FORCE_INLINE NO_DISCARD Matrix4x4 ToRelativeMatrix4x4(const Vector3<int32> &cell) const NOEXCEPT
	{
		const Vector3<int32> delta{ _Cell - cell };

		return Matrix4x4(_Position + Vector3<float32>(static_cast<float32>(delta._X), static_cast<float32>(delta._Y), static_cast<float32>(delta._Z)) * WorldSystem::Instance->GetWorldGridSize(), _Rotation, Vector3<float32>(_Scale));
	}

};