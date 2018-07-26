//Header.
#include <Entities/Sound3DEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/SoundSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(Sound3DEntity);

/*
*	Default constructor.
*/
Sound3DEntity::Sound3DEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewSound3DComponentsIndex();

	//Add this sound 3D entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
Sound3DEntity::~Sound3DEntity() NOEXCEPT
{
	//Remove this sound 3D entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& Sound3DEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetSound3DComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& Sound3DEntity::GetRotation() const NOEXCEPT
{
	//Sound 3D entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& Sound3DEntity::GetScale() const NOEXCEPT
{
	//Sound 3D entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void Sound3DEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetSound3DComponents()[componentsIndex].position += moveVector;

	//Request the sound system to update the position of this sound 3D entity.
	SoundSystem::Instance->UpdateSound3DEntityPosition(this, ComponentManager::GetSound3DComponents()[componentsIndex].position);

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void Sound3DEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void Sound3DEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this sound 3D entity.
*/
void Sound3DEntity::Initialize(const FMOD::Studio::EventDescription *const RESTRICT eventDescription) NOEXCEPT
{
	//Initialize this sound 3D entity via the Sound System.
	SoundSystem::Instance->InitializeSound3DEntity(this, eventDescription);
}