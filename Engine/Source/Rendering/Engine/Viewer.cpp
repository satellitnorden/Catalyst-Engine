//Header file.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(Viewer);

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