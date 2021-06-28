#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/ScopedLock.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystCoordinateSpaces.h>
#include <Math/General/Vector.h>

//Systems.
#include <Systems/RenderingSystem.h>

//World.
#include <World/Core/WorldTransform.h>

class ALIGN(8) Perceiver final
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
	*	Returns the world transform of the perceiver.
	*/
	FORCE_INLINE NO_DISCARD const WorldTransform &GetWorldTransform() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _WorldTransform;
	}

	/*
	*	Sets the world transform of the perceiver.
	*/
	FORCE_INLINE void SetWorldTransform(const WorldTransform &value) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_WorldTransform = value;

		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the right vector of the perceiver.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetRightVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_WorldTransform.GetRotation());
	}

	/*
	*	Returns the up vector of the perceiver.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetUpVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_WorldTransform.GetRotation());
	}

	/*
	*	Returns the forward vector of the perceiver.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetForwardVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_WorldTransform.GetRotation());
	}

	/*
	*	Returns the field of view in radians.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFieldOfView() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _FieldOfView;
	}

	/*
	*	Sets the field of view in radians.
	*/
	FORCE_INLINE void SetFieldOfView(const float32 new_field_of_view) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_FieldOfView = new_field_of_view;

		_ProjectionMatrixDirty = true;
		_PerceiverMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the near plane.
	*/
	FORCE_INLINE NO_DISCARD float32 GetNearPlane() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _NearPlane;
	}

	/*
	*	Sets the near plane.
	*/
	FORCE_INLINE void SetNearPlane(const float32 new_near_plane) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_NearPlane = new_near_plane;

		_ProjectionMatrixDirty = true;
	}

	/*
	*	Returns the far plane.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFarPlane() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _FarPlane;
	}

	/*
	*	Sets the far plane.
	*/
	FORCE_INLINE void SetFarPlane(const float32 new_far_plane) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_FarPlane = new_far_plane;

		_ProjectionMatrixDirty = true;
	}

	/*
	*	Sets the jitter of the projection matrix.
	*/
	FORCE_INLINE void SetProjectionMatrixJitter(const Vector2<float32> &new_projection_matrix_jitter) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_ProjectionMatrixJitter = new_projection_matrix_jitter;

		_ProjectionMatrixDirty = true;
	}

	/*
	*	Returns the projection matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ProjectionMatrix;
	}

	/*
	*	Returns the inverse projection matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetInverseProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InverseProjectionMatrix;
	}

	/*
	*	Returns the perceiver matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetPerceiverMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_PerceiverMatrix;
	}

	/*
	*	Returns the inverse perceiver matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetInversePerceiverMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InversePerceiverMatrix;
	}

	/*
	*	Returns the view matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetViewMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ViewMatrix;
	}

	/*
	*	Returns the frustum planes.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const StaticArray<Vector4<float32>, 6> *const RESTRICT GetFrustumPlanes() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_FrustumPlanes;
	}

private:

	//The lock for the perceiver.
	mutable Spinlock _Lock;

	//The world transform.
	WorldTransform _WorldTransform;

	//The field of view.
	float32 _FieldOfView{ 60.0f };

	//The near plane.
	float32 _NearPlane{ 0.1f };

	//The far plane.
	float32 _FarPlane{ 1'024.0f };

	//The projection matrix jitter.
	Vector2<float32> _ProjectionMatrixJitter{ 0.0f, 0.0f };

	//Denotes whether or not the projection matrix is dirty.
	bool _ProjectionMatrixDirty{ true };

	//Denotes whether or not the perceiver matrix is dirty.
	bool _PerceiverMatrixDirty{ true };

	//Denotes whether or not the frustum planes is dirty.
	bool _FrustumPlanesDirty{ true };

	//The projection matrix.
	Matrix4x4 _ProjectionMatrix;

	//The inverse projection matrix.
	Matrix4x4 _InverseProjectionMatrix;

	//The perceiver matrix.
	Matrix4x4 _PerceiverMatrix;

	//The inverse perceiver matrix.
	Matrix4x4 _InversePerceiverMatrix;

	//The view matrix.
	Matrix4x4 _ViewMatrix;

	//The frustum planes.
	StaticArray<Vector4<float32>, 6> _FrustumPlanes;

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