//Header file.
#include <Lighting/DirectionalLight.h>

//Core.
#include <Core/General/Perceiver.h>

//Lighting.
#include <Lighting/LightingCore.h>

/*
*	Updates the projection matrix.
*/
void DirectionalLight::UpdateProjectionMatrix() NOEXCEPT
{
	//Update the projection matrix.
	_ProjectionMatrix = Matrix4::Ortographic(	-LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 0.5f,
												LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 0.5f,
												-LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 0.5f,
												LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 0.5f,
												0.0f,
												LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 2.0f);

	//Reset the dirtyness of the projection matrix.
	_ProjectionMatrixDirty = false;
}

/*
*	Updates the light matrix.
*/
void DirectionalLight::UpdateLightMatrix() NOEXCEPT
{
	//Update the light matrix.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };
	const Vector3<float> directionalLightForwardVector{ Vector3<float>::ForwardVector(_Rotation) };
	const Vector3<float> directionalLightUpVector{ Vector3<float>::UpVector(_Rotation) };
	const Vector3<float> directionalLightPosition{ perceiverPosition + (directionalLightForwardVector * -1.0f) * (LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE * 1.0f) };

	_LightMatrix = Matrix4::LookAt(directionalLightPosition, directionalLightPosition + directionalLightForwardVector, directionalLightUpVector);

	//Reset the dirtyness of the light matrix.
	_LightMatrixDirty = false;
}

/*
*	Update the view matrix.
*/
void DirectionalLight::UpdateViewMatrix() NOEXCEPT
{
	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _LightMatrix;

	//Reset the dirtyness of the view matrix.
	_ViewMatrixDirty = false;
}

/*
*	Update the frustum planes.
*/
void DirectionalLight::UpdateFrustumPlanes() NOEXCEPT
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