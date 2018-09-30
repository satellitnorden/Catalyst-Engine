//Header file.
#include <Entities/CameraEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewCameraComponentsIndex(this);
}

/*
*	Returns the field of view in degrees for this camera entity.
*/
float CameraEntity::GetFieldOfViewDegrees() const NOEXCEPT
{
	//Return the field of view for this camera entity.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._FieldOfViewDegrees;
}

/*
*	Returns the field of view in radians for this camera entity.
*/
float CameraEntity::GetFieldOfViewRadians() const NOEXCEPT
{
	//Return the field of view for this camera entity.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._FieldOfViewRadians;
}

/*
*	Returns the near plane of the frustum.
*/
float CameraEntity::GetNearPlane() const NOEXCEPT
{
	//Return the near plane of the frustum.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._NearPlane;
}

/*
*	Returns the far plane of the frustum.
*/
float CameraEntity::GetFarPlane() const NOEXCEPT
{
	//Return the far plane of the frustum.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._FarPlane;
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT CameraEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT CameraEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Callback for when this entity is moved.
*/
void CameraEntity::OnMove() NOEXCEPT
{
	//Tell the rendering system to update it's matrices.
	RenderingSystem::Instance->UpdateMatrices();
}

/*
*	Callback for when this entity is rotated.
*/
void CameraEntity::OnRotate() NOEXCEPT
{
	//Tell the rendering system to update it's matrices.
	RenderingSystem::Instance->UpdateMatrices();
}