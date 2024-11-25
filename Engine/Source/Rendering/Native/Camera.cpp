//Header file.
#include <Rendering/Native/Camera.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Checks for updates.
*/
void Camera::CheckUpdates() NOEXCEPT
{
	if (_ProjectionMatrixDirty)
	{
		UpdateProjectionMatrix();
	}

	if (_CameraMatrixDirty)
	{
		UpdateCameraMatrix();
	}

	if (_FrustumDirty
#if !defined(CATALYST_CONFIGURATION_FINAL)
		&& !_FrustumLocked.Load()
#endif
		)
	{
		UpdateFrustum();
	}
}

/*
*	Updates the projection matrix.
*/
void Camera::UpdateProjectionMatrix() NOEXCEPT
{
	//Update the projection matrix.
	_ProjectionMatrix = Matrix4x4::ReversePerspective(BaseMath::DegreesToRadians(_FieldOfView), RenderingSystem::Instance->GetFullAspectRatio(), _NearPlane, _FarPlane);

	//Apply the jitter.
	_ProjectionMatrix._Matrix[2]._X -= _ProjectionMatrixJitter._X;
	_ProjectionMatrix._Matrix[2]._Y -= _ProjectionMatrixJitter._Y;

	//Update the inverse projection matrix.
	_InverseProjectionMatrix = _ProjectionMatrix;
	_InverseProjectionMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _CameraMatrix;

	//Reset the dirtyness of the projection matrix.
	_ProjectionMatrixDirty = false;
}

/*
*	Updates the camera matrix.
*/
void Camera::UpdateCameraMatrix() NOEXCEPT
{
	//Update the camera matrix.
	_CameraMatrix = Matrix4x4::LookAt(_WorldTransform.GetLocalPosition(), _WorldTransform.GetLocalPosition() + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_WorldTransform.GetRotation().ToEulerAngles()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_WorldTransform.GetRotation().ToEulerAngles()));

	//Update the inverse camera matrix.
	_InverseCameraMatrix = _CameraMatrix;
	_InverseCameraMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _CameraMatrix;

	//Reset the dirtyness of the camera matrix.
	_CameraMatrixDirty = false;
}

/*
*	Updates the frustum.
*/
void Camera::UpdateFrustum() NOEXCEPT
{
	//Construct the frustum planes.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[0][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][0]; //Left.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[1][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][0]; //Right.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[2][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][1]; //Bottom.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[3][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][1]; //Top.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[4][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][2]; //Near.
	for (uint8 i{ 4 }; i--;) _Frustum._Planes[5][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][2]; //Far.

	//Normalize the frustum planes.
	for (uint8 i{ 0 }; i < 6; ++i)
	{
		const float32 length_reciprocal{ BaseMath::InverseSquareRoot(_Frustum._Planes[i]._X * _Frustum._Planes[i]._X + _Frustum._Planes[i]._Y * _Frustum._Planes[i]._Y + _Frustum._Planes[i]._Z * _Frustum._Planes[i]._Z) };

		_Frustum._Planes[i]._X *= length_reciprocal;
		_Frustum._Planes[i]._Y *= length_reciprocal;
		_Frustum._Planes[i]._Z *= length_reciprocal;
		_Frustum._Planes[i]._W *= length_reciprocal;
	}

	//Reset the dirtyness of the frustum.
	_FrustumDirty = false;
}