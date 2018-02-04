//Header file.
#include <CameraEntity.h>

//Components.
#include <ComponentManager.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(CameraEntity);

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewCameraEntityComponentsIndex();

	//Add this camera entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
CameraEntity::~CameraEntity() CATALYST_NOEXCEPT
{
	//Remove this camera entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& CameraEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& CameraEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& CameraEntity::GetScale() CATALYST_NOEXCEPT
{
	//Camera entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void CameraEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void CameraEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void CameraEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Returns the forward vector of this entity.
*/
Vector3 CameraEntity::GetForwardVector() CATALYST_NOEXCEPT
{
	//Return the forward vector of this entity.
	return Vector3(0.0f, 0.0f, -1.0f).Rotated(GetRotation());
}

/*
*	Returns the right vector of this entity.
*/
Vector3 CameraEntity::GetRightVector() CATALYST_NOEXCEPT
{
	//Return the right vector of this entity.
	return Vector3(1.0f, 0.0f, 0.0f).Rotated(GetRotation());
}

/*
*	Returns the up vector of this entity.
*/
Vector3 CameraEntity::GetUpVector() CATALYST_NOEXCEPT
{
	//Return the up vector of this entity.
	return Vector3(0.0f, 1.0f, 0.0f).Rotated(GetRotation());
}

/*
*	Returns the field of view for this camera entity.
*/
float CameraEntity::GetFieldOfView() const CATALYST_NOEXCEPT
{
	//Return the field of view for this camera entity.
	return ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].fieldOfView;
}

/*
*	Returns the near plane of the frustum.
*/
float CameraEntity::GetNearPlane() const CATALYST_NOEXCEPT
{
	//Return the near plane of the frustum.
	return ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].nearPlane;
}

/*
*	Returns the far plane of the frustum.
*/
float CameraEntity::GetFarPlane() const CATALYST_NOEXCEPT
{
	//Return the far plane of the frustum.
	return ComponentManager::GetCameraEntityCameraComponents()[componentsIndex].farPlane;
}