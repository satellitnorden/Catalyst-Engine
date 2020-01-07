#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/RenderingSystem.h>

class Perceiver final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(Perceiver);

	/*
	*	Default constructor.
	*/
	Perceiver() NOEXCEPT
	{

	}

	/*
	*	Returns the position of the perceiver.
	*/
	const Vector3<float>& GetPosition() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _Position;
	}

	/*
	*	Moves the perceiver.
	*/
	void Move(const Vector3<float> &amount) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_Position += amount;

		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Sets the position of the perceiver.
	*/
	void SetPosition(const Vector3<float> &newPosition) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_Position = newPosition;

		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the rotation of the perceiver.
	*/
	const Vector3<float>& GetRotation() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _Rotation;
	}

	/*
	*	Rotates the perceiver.
	*/
	void Rotate(const Vector3<float> &amount) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_Rotation += amount;

		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Sets the rotation of the perceiver.
	*/
	void SetRotation(const Vector3<float> &newRotation) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_Rotation = newRotation;

		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the forward vector of the perceiver.
	*/
	Vector3<float> GetForwardVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return Vector3<float>::ForwardVector(_Rotation);
	}

	/*
	*	Returns the right vector of the perceiver.
	*/
	Vector3<float> GetRightVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return Vector3<float>::RightVector(_Rotation);
	}

	/*
	*	Returns the up vector of the perceiver.
	*/
	Vector3<float> GetUpVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return Vector3<float>::UpVector(_Rotation);
	}

	/*
	*	Returns the field of view in radians.
	*/
	float GetFieldOfView() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _FieldOfView;
	}

	/*
	*	Sets the field of view in radians.
	*/
	void SetFieldOfView(const float fieldOfView) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_FieldOfView = fieldOfView;

		_ProjectionMatrixDirty = true;
		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the near plane.
	*/
	float GetNearPlane() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _NearPlane;
	}

	/*
	*	Returns the far plane.
	*/
	float GetFarPlane() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _FarPlane;
	}

	/*
	*	Sets the far plane.
	*/
	void SetFarPlane(const float far_plane) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_ProjectionMatrixDirty = true;

		_FarPlane = far_plane;
	}

	/*
	*	Sets the jitter of the projection matrix.
	*/
	void SetProjectionMatrixJitter(const Vector2<float> &newProjectionMatrixJitter) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_ProjectionMatrixJitter = newProjectionMatrixJitter;

		_ProjectionMatrixDirty = true;
	}

	/*
	*	Returns the projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ProjectionMatrix;
	}

	/*
	*	Returns the inverse projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetInverseProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InverseProjectionMatrix;
	}

	/*
	*	Returns the perceiver matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetPerceiverMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_PerceiverMatrix;
	}

	/*
	*	Returns the inverse perceiver matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetInversePerceiverMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InversePerceiverMatrix;
	}

	/*
	*	Returns the view matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetViewMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ViewMatrix;
	}

	/*
	*	Returns the frustum planes.
	*/
	RESTRICTED const StaticArray<Vector4<float>, 6> *const RESTRICT GetFrustumPlanes() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_FrustumPlanes;
	}

private:

	//The lock for the perceiver.
	mutable Spinlock _Lock;

	//The position.
	Vector3<float> _Position{ VectorConstants::ZERO };

	//The rotation.
	Vector3<float> _Rotation{ VectorConstants::ZERO };

	//The field of view.
	float _FieldOfView{ CatalystBaseMath::DegreesToRadians(60.0f) };

	//The near plane.
	float _NearPlane{ 0.1f };

	//The far plane.
	float _FarPlane{ 1'024.0f };

	//The projection matrix jitter.
	Vector2<float> _ProjectionMatrixJitter{ 0.0f, 0.0f };

	//Denotes whether or not the projection matrix is dirty.
	bool _ProjectionMatrixDirty{ true };

	//Denotes whether or not the perceiver matrix is dirty.
	bool _PerceiverMatrixDirty{ true };

	//Denotes whether or not the frustum planes is dirty.
	bool _FrustumPlanesDirty{ true };

	//The projection matrix.
	Matrix4 _ProjectionMatrix;

	//The inverse projection matrix.
	Matrix4 _InverseProjectionMatrix;

	//The perceiver matrix.
	Matrix4 _PerceiverMatrix;

	//The inverse perceiver matrix.
	Matrix4 _InversePerceiverMatrix;

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
	*	Updates the perceiver matrix.
	*/
	void UpdatePerceiverMatrix() NOEXCEPT;

	/*
	*	Updates the frustum planes.
	*/
	void UpdateFrustumPlanes() NOEXCEPT;

};