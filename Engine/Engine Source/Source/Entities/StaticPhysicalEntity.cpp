//Header file.
#include <Entities/StaticPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(StaticPhysicalEntity);

/*
*	Default constructor.
*/
StaticPhysicalEntity::StaticPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewStaticPhysicalComponentsIndex();

	//Add this static physical entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
StaticPhysicalEntity::~StaticPhysicalEntity() NOEXCEPT
{
	//Remove this static physical entity from the universal container.
	Instances.Erase(this);
}

/*
*	Initializes this static physical entity.
*/
void StaticPhysicalEntity::Initialize(const PhysicalModel &newModel, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) NOEXCEPT
{
	//Initialize this physical entity.
	RenderingSystem::Instance->InitializeStaticPhysicalEntity(*this, newModel, position, rotation, scale);
}

/*
*	Returns the position of this entity.
*/
const Vector3& StaticPhysicalEntity::GetPosition() NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& StaticPhysicalEntity::GetRotation() NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& StaticPhysicalEntity::GetScale() NOEXCEPT
{
	//Return the scale of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[componentsIndex].scale;
}

/*
*	Moves this entity.
*/
void StaticPhysicalEntity::Move(const Vector3 &moveVector) NOEXCEPT
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
void StaticPhysicalEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void StaticPhysicalEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}