//Header file.
#include <Lighting/DirectionalLight.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

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
												LightingConstants::DIRECTIONAL_SHADOW_VIEW_DISTANCE);

	//Reset the dirtyness of the projection matrix.
	_ProjectionMatrixDirty = false;
}

/*
*	Updates the light matrix.
*/
void DirectionalLight::UpdateLightMatrix() NOEXCEPT
{
	//Update the light matrix.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };
	const Vector3 directionalLightForwardVector{ Vector3::FORWARD.Rotated(_Rotation) };
	const Vector3 directionalLightUpVector{ Vector3::UP.Rotated(_Rotation) };
	const Vector3 directionalLightPosition{ viewerPosition + (directionalLightForwardVector * -1.0f) * (RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f) };

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