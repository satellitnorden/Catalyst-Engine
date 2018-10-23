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
	_ViewerMatrix = Matrix4::LookAt(_Position, _Position + GetForwardVector(), GetUpVector());

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
	//Left frustum plane.
	_FrustumPlanes[0]._X = _ViewMatrix._Matrix[0]._X + _ViewMatrix._Matrix[3]._X;
	_FrustumPlanes[0]._X = _ViewMatrix._Matrix[0]._X + _ViewMatrix._Matrix[3]._Y;
	_FrustumPlanes[0]._X = _ViewMatrix._Matrix[0]._X + _ViewMatrix._Matrix[3]._Z;
	_FrustumPlanes[0]._X = _ViewMatrix._Matrix[0]._X + _ViewMatrix._Matrix[3]._W;

	//Reset the dirtyness of the frustum planes.
	_FrustumPlanesDirty = false;
}