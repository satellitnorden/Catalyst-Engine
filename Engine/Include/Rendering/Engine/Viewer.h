#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

//Systems.
#include <Systems/RenderingSystem.h>

class Viewer final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(Viewer);

	/*
	*	Default constructor.
	*/
	Viewer() NOEXCEPT
	{

	}

	/*
	*	Returns the position of the viewer.
	*/
	const Vector3& GetPosition() const NOEXCEPT
	{
		return _Position;
	}

	/*
	*	Moves the viewer.
	*/
	void Move(const Vector3 &amount) NOEXCEPT
	{
		_Position += amount;

		RenderingSystem::Instance->UpdateMatrices();
	}

	/*
	*	Sets the position of the viewer.
	*/
	void SetPosition(const Vector3 &newPosition) NOEXCEPT
	{
		_Position = newPosition;

		RenderingSystem::Instance->UpdateMatrices();
	}

	/*
	*	Returns the rotation of the viewer.
	*/
	const Vector3& GetRotation() const NOEXCEPT
	{
		return _Rotation;
	}

	/*
	*	Rotates the viewer.
	*/
	void Rotate(const Vector3 &amount) NOEXCEPT
	{
		_Rotation += amount;

		RenderingSystem::Instance->UpdateMatrices();
	}

	/*
	*	Sets the rotation of the viewer.
	*/
	void SetRotation(const Vector3 &newRotation) NOEXCEPT
	{
		_Rotation = newRotation;

		RenderingSystem::Instance->UpdateMatrices();
	}

	/*
	*	Returns the forward vector of the viewer.
	*/
	Vector3 GetForwardVector() const NOEXCEPT
	{
		return Vector3::Normalize(Vector3(Vector3::FORWARD).Rotated(_Rotation));
	}

	/*
	*	Returns the right vector of the viewer.
	*/
	Vector3 GetRightVector() const NOEXCEPT
	{
		return Vector3::Normalize(Vector3(Vector3::RIGHT).Rotated(_Rotation));
	}

	/*
	*	Returns the up vector of the viewer.
	*/
	Vector3 GetUpVector() const NOEXCEPT
	{
		return Vector3::Normalize(Vector3(Vector3::UP).Rotated(_Rotation));
	}

	/*
	*	Returns the field of view in degrees
	*/
	float GetFieldOfViewDegrees() const NOEXCEPT
	{
		return _FieldOfViewDegrees;
	}

	/*
	*	Returns the field of view in radians.
	*/
	float GetFieldOfViewRadians() const NOEXCEPT
	{
		return _FieldOfViewRadians;
	}

	/*
	*	Returns the near plane.
	*/
	float GetNearPlane() const NOEXCEPT
	{
		return _NearPlane;
	}

	/*
	*	Returns the far plane.
	*/
	float GetFarPlane() const NOEXCEPT
	{
		return _FarPlane;
	}

private:

	//The position.
	Vector3 _Position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The field of view in degrees.
	float _FieldOfViewDegrees{ 60.0f };

	//The field of view in radians.
	float _FieldOfViewRadians{ CatalystBaseMath::DegreesToRadians(60.0f) };

	//The near plane.
	float _NearPlane{ 0.1f };

	//The far plane.
	float _FarPlane{ 419'904.0f };

};