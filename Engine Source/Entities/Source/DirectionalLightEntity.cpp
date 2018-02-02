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
DirectionalLightEntity::DirectionalLightEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewDirectionalLightEntityComponentsIndex();

	//Add this directional light entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
DirectionalLightEntity::~DirectionalLightEntity() CATALYST_NOEXCEPT
{
	//Remove this directional light entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
Vector3& DirectionalLightEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Directional light entities has no scale.
	static Vector3 defaultPosition{ 0.0f, 0.0f, 0.0f };

	return defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
Vector3& DirectionalLightEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetDirectionalLightEntityDirectionalLightComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
Vector3& DirectionalLightEntity::GetScale() CATALYST_NOEXCEPT
{
	//Point light entities has no scale.
	static Vector3 defaultScale{ 1.0f, 1.0f, 1.0f };

	return defaultScale;
}

/*
*	Moves this entity.
*/
void DirectionalLightEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
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
void DirectionalLightEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetDirectionalLightEntityDirectionalLightComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void DirectionalLightEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
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
void DirectionalLightEntity::SetIntensity(const float newIntensity) CATALYST_NOEXCEPT
{
	//Set the intensity of this directional light entity.
	ComponentManager::GetDirectionalLightEntityDirectionalLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the color of this directional light entity.
*/
void DirectionalLightEntity::SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT
{

}