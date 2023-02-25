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

	if (_FrustumPlanesDirty)
	{
		UpdateFrustumPlanes();
	}
}

/*
*	Updates the projection matrix.
*/
void Camera::UpdateProjectionMatrix() NOEXCEPT
{
	//Update the projection matrix.
	_ProjectionMatrix = Matrix4x4::ReversePerspective(CatalystBaseMath::DegreesToRadians(_FieldOfView), RenderingSystem::Instance->GetFullAspectRatio(), _NearPlane, _FarPlane);

	//Apply the jitter.
	_ProjectionMatrix._Matrix[2]._X += _ProjectionMatrixJitter._X;
	_ProjectionMatrix._Matrix[2]._Y += _ProjectionMatrixJitter._Y;

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
*	Updates the frustum planes.
*/
void Camera::UpdateFrustumPlanes() NOEXCEPT
{
	//Construct the frustum planes.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[0][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][0]; //Left.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[1][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][0]; //Right.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[2][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][1]; //Bottom.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[3][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][1]; //Top.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[4][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][2]; //Near.
	for (uint8 i{ 4 }; i--;) _FrustumPlanes[5][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][2]; //Far.

	//Normalize the frustum planes.
	for (uint8 i{ 0 }; i < 6; ++i)
	{
		const float32 length{ CatalystBaseMath::SquareRoot(_FrustumPlanes[i]._X * _FrustumPlanes[i]._X + _FrustumPlanes[i]._Y * _FrustumPlanes[i]._Y + _FrustumPlanes[i]._Z * _FrustumPlanes[i]._Z) };
		const float32 length_reciprocal{ 1.0f / length };

		_FrustumPlanes[i]._X *= length_reciprocal;
		_FrustumPlanes[i]._Y *= length_reciprocal;
		_FrustumPlanes[i]._Z *= length_reciprocal;
		_FrustumPlanes[i]._W *= length_reciprocal;
	}

	//Reset the dirtyness of the frustum planes.
	_FrustumPlanesDirty = false;
}