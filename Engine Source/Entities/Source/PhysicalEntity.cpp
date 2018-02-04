//Header file.
#include <PhysicalEntity.h>

//Components.
#include <ComponentManager.h>

//Systems.
#include <GraphicsSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(PhysicalEntity);

/*
*	Default constructor.
*/
PhysicalEntity::PhysicalEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewPhysicalEntityComponentsIndex();

	//Add this physical entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
PhysicalEntity::~PhysicalEntity() CATALYST_NOEXCEPT
{
	//Remove this physical entity from the universal container.
	Instances.Erase(this);
}

/*
*	Initializes this physical entity.
*/
void PhysicalEntity::Initialize(const PhysicalModel &newModel) CATALYST_NOEXCEPT
{
	//Initialize this physical entity.
	GraphicsSystem::Instance->InitializePhysicalEntity(*this, newModel);
}

/*
*	Returns the position of this entity.
*/
const Vector3& PhysicalEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& PhysicalEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& PhysicalEntity::GetScale() CATALYST_NOEXCEPT
{
	//Return the scale of this entity.
	return ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].scale;
}

/*
*	Moves this entity.
*/
void PhysicalEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void PhysicalEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void PhysicalEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
{
	//Scale this physical entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].scale *= scaleVector;

	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}