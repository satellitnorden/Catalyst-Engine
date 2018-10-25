#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>
#include <Math/Matrix4.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/Spinlock.h>

class DirectionalLight final
{

public:

	/*
	*	Returns the projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetProjectionMatrix() NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		if (_ProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		return &_ProjectionMatrix;
	}

	/*
	*	Returns the light matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetLightMatrix() NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		if (_LightMatrixDirty)
		{
			UpdateLightMatrix();
		}

		return &_LightMatrix;
	}

	/*
	*	Returns the view matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetViewMatrix() NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		if (_ProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		if (_LightMatrixDirty)
		{
			UpdateLightMatrix();
		}

		if (_ViewMatrixDirty)
		{
			UpdateViewMatrix();
		}

		return &_ViewMatrix;
	}

	/*
	*	Returns the direction of the directional light.
	*/
	const Vector3& GetDirection() const NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		return Vector3::FORWARD.Rotated(_Rotation);
	}

	/*
	*	Sets the rotation of the directional light.
	*/
	void SetRotation(const Vector3 &newRotation) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		_Rotation = newRotation;

		_LightMatrixDirty = true;
		_ViewMatrixDirty = true;
	}

	/*
	*	Returns the color of the directional light.
	*/
	const Vector3& GetColor() const NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		return _Color;
	}

	/*
	*	Sets the color of the directional light.
	*/
	void SetColor(const Vector3 &newColor) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		_Color = newColor;
	}

	/*
	*	Returns the intensity of the directional light.
	*/
	float GetIntensity() const NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		return _Intensity;
	}

	/*
	*	Sets the intensity of the directional light.
	*/
	void SetIntensity(const float newIntensity) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		_Intensity = newIntensity;
	}

private:

	//The lock.
	mutable Spinlock _Lock;

	//Denotes whether or not the projection matrix is dirty.
	bool _ProjectionMatrixDirty{ true };

	//The projection matrix.
	Matrix4 _ProjectionMatrix;

	//Denotes whether or not the light matrix is dirty.
	bool _LightMatrixDirty{ true };

	//The light matrix.
	Matrix4 _LightMatrix;

	//Denotes whether or not the view matrix is dirty.
	bool _ViewMatrixDirty{ true };

	//The view matrix.
	Matrix4 _ViewMatrix;

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The color.
	Vector3 _Color{ 1.0f, 1.0f, 1.0f };

	//The intensity.
	float _Intensity{ 1.0f };

	/*
	*	Updates the projection matrix.
	*/
	void UpdateProjectionMatrix() NOEXCEPT;

	/*
	*	Updates the light matrix.
	*/
	void UpdateLightMatrix() NOEXCEPT;

	/*
	*	Update the view matrix.
	*/
	void UpdateViewMatrix() NOEXCEPT;

};