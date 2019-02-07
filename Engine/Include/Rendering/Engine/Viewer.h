#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

//Multithreading.
#include <Multithreading/ScopedReadLock.h>
#include <Multithreading/ScopedWriteLock.h>

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
	const Vector3<float>& GetPosition() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _Position;
	}

	/*
	*	Moves the viewer.
	*/
	void Move(const Vector3<float> &amount) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Position += amount;

		_ViewerMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Sets the position of the viewer.
	*/
	void SetPosition(const Vector3<float> &newPosition) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Position = newPosition;

		_ViewerMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the rotation of the viewer.
	*/
	const Vector3<float>& GetRotation() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _Rotation;
	}

	/*
	*	Rotates the viewer.
	*/
	void Rotate(const Vector3<float> &amount) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Rotation += amount;

		_ViewerMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Sets the rotation of the viewer.
	*/
	void SetRotation(const Vector3<float> &newRotation) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		_Rotation = newRotation;

		_ViewerMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the forward vector of the viewer.
	*/
	Vector3<float> GetForwardVector() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return Vector3<float>::Normalize(Vector3<float>(Vector3<float>::FORWARD).Rotated(_Rotation));
	}

	/*
	*	Returns the right vector of the viewer.
	*/
	Vector3<float> GetRightVector() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return Vector3<float>::Normalize(Vector3<float>(Vector3<float>::RIGHT).Rotated(_Rotation));
	}

	/*
	*	Returns the up vector of the viewer.
	*/
	Vector3<float> GetUpVector() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return Vector3<float>::Normalize(Vector3<float>(Vector3<float>::UP).Rotated(_Rotation));
	}

	/*
	*	Returns the field of view in degrees
	*/
	float GetFieldOfViewDegrees() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _FieldOfViewDegrees;
	}

	/*
	*	Returns the field of view in radians.
	*/
	float GetFieldOfViewRadians() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _FieldOfViewRadians;
	}

	/*
	*	Returns the near plane.
	*/
	float GetNearPlane() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _NearPlane;
	}

	/*
	*	Returns the far plane.
	*/
	float GetFarPlane() const NOEXCEPT
	{
		ScopedReadLock<Spinlock> scopedLock{ _Lock };

		return _FarPlane;
	}

	/*
	*	Returns the projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetProjectionMatrix() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_ProjectionMatrix;
	}

	/*
	*	Returns the inverse projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetInverseProjectionMatrix() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_InverseProjectionMatrix;
	}

	/*
	*	Returns the viewer matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetViewerMatrix() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_ViewerMatrix;
	}

	/*
	*	Returns the inverse viewer matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetInverseViewerMatrix() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_InverseViewerMatrix;
	}

	/*
	*	Returns the view matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetViewMatrix() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_ViewMatrix;
	}

	/*
	*	Returns the frustum planes.
	*/
	RESTRICTED const StaticArray<Vector4<float>, 6> *const RESTRICT GetFrustumPlanes() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ _Lock };

		CheckUpdates();

		return &_FrustumPlanes;
	}

private:

	//The lock for the viewer.
	mutable Spinlock _Lock;

	//The position.
	Vector3<float> _Position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3<float> _Rotation{ 0.0f, 0.0f, 0.0f };

	//The field of view in degrees.
	float _FieldOfViewDegrees{ 60.0f };

	//The field of view in radians.
	float _FieldOfViewRadians{ CatalystBaseMath::DegreesToRadians(60.0f) };

	//The near plane.
	float _NearPlane{ 0.1f };

	//The far plane.
	float _FarPlane{ 196'608.0f };

	//Denotes whether or not the projection matrix is dirty.
	bool _ProjectionMatrixDirty{ true };

	//Denotes whether or not the viewer matrix is dirty.
	bool _ViewerMatrixDirty{ true };

	//Denotes whether or not the frustum planes is dirty.
	bool _FrustumPlanesDirty{ true };

	//The projection matrix.
	Matrix4 _ProjectionMatrix;

	//The inverse projection matrix.
	Matrix4 _InverseProjectionMatrix;

	//The viewer matrix.
	Matrix4 _ViewerMatrix;

	//The inverse viewer matrix.
	Matrix4 _InverseViewerMatrix;

	//The view matrix.
	Matrix4 _ViewMatrix;

	StaticArray<Vector4<float>, 6> _FrustumPlanes;

	/*
	*	Checks for updates.
	*/
	void CheckUpdates() NOEXCEPT;

	/*
	*	Updates the projection matrix.	
	*/
	void UpdateProjectionMatrix() NOEXCEPT;

	/*
	*	Updates the viewer matrix.
	*/
	void UpdateViewerMatrix() NOEXCEPT;

	/*
	*	Updates the frustum planes.
	*/
	void UpdateFrustumPlanes() NOEXCEPT;

};