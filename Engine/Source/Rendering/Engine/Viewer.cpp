//Header file.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(Viewer);

/*
*	Checks for updates.
*/
void Viewer::CheckUpdates() NOEXCEPT
{
	if (_ProjectionMatrixDirty)
	{
		UpdateProjectionMatrix();
	}

	if (_ViewerMatrixDirty)
	{
		UpdateViewerMatrix();
	}

	if (_FrustumPlanesDirty)
	{
		UpdateFrustumPlanes();
	}
}

/*
*	Updates the projection matrix.
*/
void Viewer::UpdateProjectionMatrix() NOEXCEPT
{
	//Update the projection matrix.
	_ProjectionMatrix = Matrix4::ReversePerspective(_FieldOfViewRadians, static_cast<float>(RenderingSystem::Instance->GetResolution()._Width) / static_cast<float>(RenderingSystem::Instance->GetResolution()._Height), _NearPlane, _FarPlane);

	//Update the inverse projection matrix.
	_InverseProjectionMatrix = _ProjectionMatrix;
	_InverseProjectionMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _ViewerMatrix;

	//Reset the dirtyness of the projection matrix.
	_ProjectionMatrixDirty = false;
}

/*
*	Updates the viewer matrix.
*/
void Viewer::UpdateViewerMatrix() NOEXCEPT
{
	//Update the viewer matrix.
	_ViewerMatrix = Matrix4::LookAt(_Position, _Position + Vector3<float>::Normalize(Vector3<float>(Vector3<float>::FORWARD).Rotated(_Rotation)), Vector3<float>::Normalize(Vector3<float>(Vector3<float>::UP).Rotated(_Rotation)));

	//Update the inverse viewer matrix.
	_InverseViewerMatrix = _ViewerMatrix;
	_InverseViewerMatrix.Inverse();

	//Update the view matrix.
	_ViewMatrix = _ProjectionMatrix * _ViewerMatrix;

	//Reset the dirtyness of the viewer matrix.
	_ViewerMatrixDirty = false;
}

/*
*	Updates the frustum planes.
*/
void Viewer::UpdateFrustumPlanes() NOEXCEPT
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