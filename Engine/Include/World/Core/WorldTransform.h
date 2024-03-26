#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>
#include <Math/General/Quaternion.h>

//World.
#include <World/Core/WorldPosition.h>

class WorldTransform final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE explicit WorldTransform() NOEXCEPT
		:
		_WorldPosition(),
		_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
		_Scale(1.0f, 1.0f, 1.0f)
	{

	}

	/*
	*	Constructor taking world position, quaternion and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const WorldPosition &initial_world_position,
											const Quaternion &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_world_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking world position, euler angles and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const WorldPosition &initial_world_position,
											const EulerAngles &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_world_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking cell, local position, euler angles and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const Vector3<int32> &initial_cell,
											const Vector3<float32> &initial_local_position,
											const Quaternion &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_cell, initial_local_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking cell, local position, euler angles and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const Vector3<int32> &initial_cell,
											const Vector3<float32> &initial_local_position,
											const EulerAngles &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_cell, initial_local_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking local position, quaternion and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const Vector3<float32> &initial_local_position,
											const Quaternion &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_local_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking local position, euler angles and scale.
	*/
	FORCE_INLINE explicit WorldTransform(	const Vector3<float32> &initial_local_position,
											const EulerAngles &initial_rotation,
											const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_WorldPosition(initial_local_position),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

	/*
	*	Constructor taking a Matrix4x4 transformation.
	*/
	FORCE_INLINE explicit WorldTransform(const Matrix4x4 &transformation) NOEXCEPT
		:
		_WorldPosition(transformation.GetTranslation()),
		_Rotation(transformation.GetRotation()),
		_Scale(transformation.GetScale())
	{

	}
		
	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE NO_DISCARD bool operator==(const WorldTransform &other) const NOEXCEPT
	{
		return Memory::Compare(this, &other, sizeof(WorldTransform));
	}

	/*
	*	Inequality operator overload.
	*/
	FORCE_INLINE NO_DISCARD bool operator!=(const WorldTransform &other) const NOEXCEPT
	{
		return !operator==(other);
	}

	/*
	*	Returns the world position.
	*/
	FORCE_INLINE NO_DISCARD const WorldPosition& GetWorldPosition() const NOEXCEPT
	{
		return _WorldPosition;
	}

	/*
	*	Sets the world position.
	*/
	FORCE_INLINE void SetWorldPosition(const WorldPosition &value) NOEXCEPT
	{
		_WorldPosition = value;
	}

	/*
	*	Returns the cell.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<int32> &GetCell() const NOEXCEPT
	{
		return _WorldPosition.GetCell();
	}

	/*
	*	Sets the cell.
	*/
	FORCE_INLINE void SetCell(const Vector3<int32> &value) NOEXCEPT
	{
		_WorldPosition.SetCell(value);
	}

	/*
	*	Returns the local position.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<float32> &GetLocalPosition() const NOEXCEPT
	{
		return _WorldPosition.GetLocalPosition();
	}

	/*
	*	Sets the local position.
	*/
	FORCE_INLINE void SetLocalPosition(const Vector3<float32> &value) NOEXCEPT
	{
		_WorldPosition.SetLocalPosition(value);
	}

	/*
	*	Returns the absolute position.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetAbsolutePosition() const NOEXCEPT
	{	
		return _WorldPosition.GetAbsolutePosition();
	}

	/*
	*	Sets the absolute position.
	*/
	FORCE_INLINE void SetAbsolutePosition(const Vector3<float32>& value) NOEXCEPT
	{
		_WorldPosition.SetAbsolutePosition(value);
	}

	/*
	*	Returns the relative position.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetRelativePosition(const Vector3<int32> &cell) const NOEXCEPT
	{	
		return _WorldPosition.GetRelativePosition(cell);
	}

	/*
	*	Sets the relative position.
	*/
	FORCE_INLINE void SetRelativePosition(const Vector3<int32> &cell, const Vector3<float32> &value) NOEXCEPT
	{
		_WorldPosition.SetRelativePosition(cell, value);
	}

	/*
	*	Returns the rotation.
	*/
	FORCE_INLINE NO_DISCARD const Quaternion &GetRotation() const NOEXCEPT
	{
		return _Rotation;
	}

	/*
	*	Sets the rotation.
	*/
	FORCE_INLINE void SetRotation(const Quaternion &value) NOEXCEPT
	{
		_Rotation = value;
	}

	/*
	*	Sets the rotation.
	*/
	FORCE_INLINE void SetRotation(const EulerAngles &value) NOEXCEPT
	{
		_Rotation = value;
	}

	/*
	*	Returns the scale.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<float32> &GetScale() const NOEXCEPT
	{
		return _Scale;
	}

	/*
	*	Sets the scale.
	*/
	FORCE_INLINE void SetScale(const Vector3<float32> &value) NOEXCEPT
	{
		_Scale = value;
	}

	/*
	*	Converts this world transform into a local Matrix4x4.
	*/
	FORCE_INLINE NO_DISCARD Matrix4x4 ToLocalMatrix4x4() const NOEXCEPT
	{
		return Matrix4x4(_WorldPosition.GetLocalPosition(), _Rotation, _Scale);
	}

	/*
	*	Converts this world transform into an absolute Matrix4x4.
	*/
	NO_DISCARD Matrix4x4 ToAbsoluteMatrix4x4() const NOEXCEPT;

	/*
	*	Converts this world transform into a relative Matrix4x4 as seen from the given cell.
	*/
	NO_DISCARD Matrix4x4 ToRelativeMatrix4x4(const Vector3<int32> &cell) const NOEXCEPT;

	/*
	*	Returns the forward vector.
	*/
	NO_DISCARD Vector3<float32> GetForwardVector() const NOEXCEPT;

private:

	//The world position.
	WorldPosition _WorldPosition;

	//The rotation.
	Quaternion _Rotation;

	//The scale.
	Vector3<float32> _Scale;

};