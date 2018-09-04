//Header file.
#include <Entities/CameraEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewCameraComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& CameraEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& CameraEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& CameraEntity::GetScale() const NOEXCEPT
{
	//Camera entities has no scale.
	return Entity::_DefaultScale;
}

/*
*	Moves this entity.
*/
void CameraEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Position += moveVector;

	//Move all children.
	for (auto child : _Children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void CameraEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._Rotation += rotateVector;

	//Rotate all children.
	for (auto child : _Children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void CameraEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : _Children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Returns the forward vector of this entity.
*/
Vector3 CameraEntity::GetForwardVector() const NOEXCEPT
{
	//Return the forward vector of this entity.
	return Vector3(0.0f, 0.0f, -1.0f).Rotated(GetRotation());
}

/*
*	Returns the right vector of this entity.
*/
Vector3 CameraEntity::GetRightVector() const NOEXCEPT
{
	//Return the right vector of this entity.
	return Vector3(1.0f, 0.0f, 0.0f).Rotated(GetRotation());
}

/*
*	Returns the up vector of this entity.
*/
Vector3 CameraEntity::GetUpVector() const NOEXCEPT
{
	//Return the up vector of this entity.
	return Vector3(0.0f, 1.0f, 0.0f).Rotated(GetRotation());
}

/*
*	Returns the field of view for this camera entity.
*/
float CameraEntity::GetFieldOfView() const NOEXCEPT
{
	//Return the field of view for this camera entity.
	return ComponentManager::GetCameraCameraComponents()[_ComponentsIndex]._FieldOfView;
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