#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/ScopedLock.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//World.
#include <World/Core/WorldTransform.h>

class Camera final
{

public:

	/*
	*	Default constructor.
	*/
	Camera() NOEXCEPT
	{

	}

	/*
	*	Returns the world transform.
	*/
	FORCE_INLINE NO_DISCARD const WorldTransform& GetWorldTransform() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _WorldTransform;
	}

	/*
	*	Sets the world transform.
	*/
	FORCE_INLINE void SetWorldTransform(const WorldTransform& value) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_WorldTransform = value;

		_CameraMatrixDirty = true;
		_FrustumPlanesDirty = true;
	}

	/*
	*	Returns the right vector.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetRightVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_WorldTransform.GetRotation().ToEulerAngles());
	}

	/*
	*	Returns the up vector.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetUpVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_WorldTransform.GetRotation().ToEulerAngles());
	}

	/*
	*	Returns the forward vector.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetForwardVector() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_WorldTransform.GetRotation().ToEulerAngles());
	}

	/*
	*	Returns the field of view, in degrees.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFieldOfView() const NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		return _FieldOfView;
	}

	/*
	*	Sets the field of view, in degrees.
	*/
	FORCE_INLINE void SetFieldOfView(const float32 new_field_of_view) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_FieldOfView = new_field_of_view;

		_ProjectionMatrixDirty = true;
		_CameraMatrixDirty = true;
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
	FORCE_INLINE void SetProjectionMatrixJitter(const Vector2<float32>& new_projection_matrix_jitter) NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		_ProjectionMatrixJitter = new_projection_matrix_jitter;

		_ProjectionMatrixDirty = true;
	}

	/*
	*	Returns the projection matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4* const RESTRICT GetProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ProjectionMatrix;
	}

	/*
	*	Returns the inverse projection matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4* const RESTRICT GetInverseProjectionMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InverseProjectionMatrix;
	}

	/*
	*	Returns the camera matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4* const RESTRICT GetCameraMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_CameraMatrix;
	}

	/*
	*	Returns the inverse camera matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4* const RESTRICT GetInverseCameraMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_InverseCameraMatrix;
	}

	/*
	*	Returns the view matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Matrix4x4* const RESTRICT GetViewMatrix() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_ViewMatrix;
	}

	/*
	*	Returns the frustum planes.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const StaticArray<Vector4<float32>, 6>* const RESTRICT GetFrustumPlanes() NOEXCEPT
	{
		SCOPED_LOCK(_Lock);

		CheckUpdates();

		return &_FrustumPlanes;
	}

	/*
	*	Returns the aperture.
	*/
	FORCE_INLINE NO_DISCARD float32 GetAperture() const NOEXCEPT
	{
		return _Aperture;
	}

	/*
	*	Sets the aperture.
	*/
	FORCE_INLINE void SetAperture(const float32 value) NOEXCEPT
	{
		_Aperture = value;
	}

	/*
	*	Returns the focal distance.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFocalDistance() const NOEXCEPT
	{
		return _FocalDistance;
	}

	/*
	*	Sets the focal distance.
	*/
	FORCE_INLINE void SetFocalDistance(const float32 value) NOEXCEPT
	{
		_FocalDistance = value;
	}

	/*
	*	Returns whether or not to this camera should use automatic focal distance.
	*/
	FORCE_INLINE NO_DISCARD bool GetAutomaticFocalDistance() const NOEXCEPT
	{
		return _AutomaticFocalDistance;
	}

	/*
	*	Sets whether or not to this camera should use automatic focal distance.
	*/
	FORCE_INLINE void SetAutomaticFocalDistance(const bool value) NOEXCEPT
	{
		_AutomaticFocalDistance = value;
	}

private:

	//The lock.
	mutable Spinlock _Lock;

	//The world transform.
	WorldTransform _WorldTransform;

	//The field of view, in degrees.
	float32 _FieldOfView{ 60.0f };

	//The near plane.
	float32 _NearPlane{ 0.1f };

	//The far plane.
	float32 _FarPlane{ 1'024.0f };

	//The projection matrix jitter.
	Vector2<float32> _ProjectionMatrixJitter{ 0.0f, 0.0f };

	//Denotes whether or not the projection matrix is dirty.
	bool _ProjectionMatrixDirty{ true };

	//Denotes whether or not the camera matrix is dirty.
	bool _CameraMatrixDirty{ true };

	//Denotes whether or not the frustum planes is dirty.
	bool _FrustumPlanesDirty{ true };

	//The projection matrix.
	Matrix4x4 _ProjectionMatrix;

	//The inverse projection matrix.
	Matrix4x4 _InverseProjectionMatrix;

	//The camera matrix.
	Matrix4x4 _CameraMatrix;

	//The inverse camera matrix.
	Matrix4x4 _InverseCameraMatrix;

	//The view matrix.
	Matrix4x4 _ViewMatrix;

	//The frustum planes.
	StaticArray<Vector4<float32>, 6> _FrustumPlanes;

	/*
	*	The aperture.
	*	Controls the size of depth of field,
	*	with lower values creating larger depth of field and higher values creating less depth of field.
	*	Expressed in the range [0.0f, 1.0f].
	*/
	float32 _Aperture{ 1.0f };

	//The focal distance.
	float32 _FocalDistance{ 1.0f };

	//Denotes whether or not to this camera should use automatic focal distance.
	bool _AutomaticFocalDistance{ false };

	/*
	*	Checks for updates.
	*/
	void CheckUpdates() NOEXCEPT;

	/*
	*	Updates the projection matrix.	
	*/
	void UpdateProjectionMatrix() NOEXCEPT;

	/*
	*	Updates the camera matrix.
	*/
	void UpdateCameraMatrix() NOEXCEPT;

	/*
	*	Updates the frustum planes.
	*/
	void UpdateFrustumPlanes() NOEXCEPT;

};