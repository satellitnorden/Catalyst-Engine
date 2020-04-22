//Header file.
#include <Core/General/Perceiver.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(Perceiver);

/*
*	Checks for updates.
*/
void Perceiver::CheckUpdates() NOEXCEPT
{
	if (_ProjectionMatrixDirty)
	{
		UpdateProjectionMatrix();
	}

	if (_PerceiverMatrixDirty)
	{
		UpdatePerceiverMatrix();
	}

	if (_FrustumPlanesDirty)
	{
		UpdateFrustumPlanes();
	}
}

/*
*	Updates the projection matrix.
*/
void Perceiver::UpdateProjectionMatrix() NOEXCEPT
{
	//Update the projection matrix.
	_ProjectionMatrix = Matrix4x4::ReversePerspective(_FieldOfView, static_cast<float>(RenderingSystem::Instance->GetScaledResolution(0)._Width) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution(0)._Height), _NearPlane, _FarPlane);

	//Apply the jitter.
	_ProjectionMatrix._Matrix[2]._X += _ProjectionMatrixJitter._X;
	_ProjectionMatrix._Matrix[2]._Y += _ProjectionMatrixJitter._Y;

	//Update the inverse projection matrix.
	_InverseProjectionMatrix = _ProjectionMatrix;
	_InverseProjectionMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _PerceiverMatrix;

	//Reset the dirtyness of the projection matrix.
	_ProjectionMatrixDirty = false;
}

/*
*	Updates the perceiver matrix.
*/
void Perceiver::UpdatePerceiverMatrix() NOEXCEPT
{
	//Update the perceiver matrix.
	_PerceiverMatrix = Matrix4x4::LookAt(_Position, _Position + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_Rotation));

	//Update the inverse perceiver matrix.
	_InversePerceiverMatrix = _PerceiverMatrix;
	_InversePerceiverMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _PerceiverMatrix;

	//Reset the dirtyness of the perceiver matrix.
	_PerceiverMatrixDirty = false;
}

/*
*	Updates the frustum planes.
*/
void Perceiver::UpdateFrustumPlanes() NOEXCEPT
{
	//Construct the frustum planes.
	for (uint8 i = 4; i--;) _FrustumPlanes[0][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][0]; //Left.
	for (uint8 i = 4; i--;) _FrustumPlanes[1][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][0]; //Right.
	for (uint8 i = 4; i--;) _FrustumPlanes[2][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][1]; //Bottom.
	for (uint8 i = 4; i--;) _FrustumPlanes[3][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][1]; //Top.
	for (uint8 i = 4; i--;) _FrustumPlanes[4][i] = _ViewMatrix._Matrix[i][3] + _ViewMatrix._Matrix[i][2]; //Near.
	for (uint8 i = 4; i--;) _FrustumPlanes[5][i] = _ViewMatrix._Matrix[i][3] - _ViewMatrix._Matrix[i][2]; //Far.

	//Normalize the frustum planes.
	for (uint8 i = 0; i < 6; ++i)
	{
		const float length{ CatalystBaseMath::SquareRoot(_FrustumPlanes[i]._X * _FrustumPlanes[i]._X + _FrustumPlanes[i]._Y * _FrustumPlanes[i]._Y + _FrustumPlanes[i]._Z * _FrustumPlanes[i]._Z) };
		const float inverseLength{ 1.0f / length };

		_FrustumPlanes[i]._X *= inverseLength;
		_FrustumPlanes[i]._Y *= inverseLength;
		_FrustumPlanes[i]._Z *= inverseLength;
		_FrustumPlanes[i]._W *= inverseLength;
	}

	//Reset the dirtyness of the frustum planes.
	_FrustumPlanesDirty = false;
}