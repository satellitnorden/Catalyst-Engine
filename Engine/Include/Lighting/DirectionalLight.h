#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Multithreading.
#include <Multithreading/ScopedReadLock.h>
#include <Multithreading/ScopedWriteLock.h>
#include <Multithreading/Spinlock.h>

class DirectionalLight final
{

public:

	/*
	*	Returns the color of the directional light.
	*/
	const Vector3<float>& GetColor() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _Color;
	}

	/*
	*	Sets the color of the directional light.
	*/
	void SetColor(const Vector3<float> &newColor) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Color = newColor;
	}

	/*
	*	Sets the rotation of the directional light.
	*/
	void SetRotation(const Vector3<float> &newRotation) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Rotation = newRotation;
	}

	/*
	*	Returns the intensity of the directional light.
	*/
	float GetIntensity() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _Intensity;
	}

	/*
	*	Sets the intensity of the directional light.
	*/
	void SetIntensity(const float newIntensity) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Intensity = newIntensity;
	}

	/*
	*	Returns the direction of the directional light.
	*/
	Vector3<float> GetDirection() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return Vector3<float>::ForwardVector(_Rotation);
	}

private:

	//The lock.
	mutable Spinlock _Lock;

	//The color.
	Vector3<float> _Color{ 1.0f, 1.0f, 1.0f };

	//The rotation.
	Vector3<float> _Rotation{ VectorConstants::ZERO };

	//The intensity.
	float _Intensity{ 1.0f };

};