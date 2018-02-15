//Header file.
#include <DirectionalLightEntity.h>

//Components.
#include <ComponentManager.h>

//Math.
#include <Vector3.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(DirectionalLightEntity);

/*
*	Default constructor.
*/
DirectionalLightEntity::DirectionalLightEntity() NOEXCEPT
{
	//Get a new component index.
	componentsIndex = ComponentManager::GetNewDirectionalLightComponentsIndex();

	//Add this directional light entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
DirectionalLightEntity::~DirectionalLightEntity() NOEXCEPT
{
	//Remove this directional light entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& DirectionalLightEntity::GetPosition() NOEXCEPT
{
	//Directional light entities has no scale.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& DirectionalLightEntity::GetRotation() NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetDirectionalLightComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& DirectionalLightEntity::GetScale() NOEXCEPT
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
	ComponentManager::GetDirectionalLightComponents()[componentsIndex].rotation += rotateVector;

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
	ComponentManager::GetDirectionalLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the color of this directional light entity.
*/
void DirectionalLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{

}