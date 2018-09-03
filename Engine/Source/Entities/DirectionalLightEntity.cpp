//Header file.
#include <Entities/DirectionalLightEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/Vector3.h>

/*
*	Default constructor.
*/
DirectionalLightEntity::DirectionalLightEntity() NOEXCEPT
{
	//Get a new component index.
	componentsIndex = ComponentManager::GetNewDirectionalLightComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& DirectionalLightEntity::GetPosition() const NOEXCEPT
{
	//Directional light entities has no scale.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& DirectionalLightEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetDirectionalLightDirectionalLightComponents()[componentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& DirectionalLightEntity::GetScale() const NOEXCEPT
{
	//Point light entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void DirectionalLightEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void DirectionalLightEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetDirectionalLightDirectionalLightComponents()[componentsIndex]._Rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void DirectionalLightEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Sets the intensity of this directional light entity.
*/
void DirectionalLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity of this directional light entity.
	ComponentManager::GetDirectionalLightDirectionalLightComponents()[componentsIndex]._Intensity = newIntensity;
}

/*
*	Sets the color of this directional light entity.
*/
void DirectionalLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{

}