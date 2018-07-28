//Header.
#include <Entities/Sound2DEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/SoundSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(Sound2DEntity);

/*
*	Default constructor.
*/
Sound2DEntity::Sound2DEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewSound2DComponentsIndex();

	//Add this sound 2D entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
Sound2DEntity::~Sound2DEntity() NOEXCEPT
{
	//Remove this sound 2D entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& Sound2DEntity::GetPosition() const NOEXCEPT
{
	//Sound 2D entities has no position.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& Sound2DEntity::GetRotation() const NOEXCEPT
{
	//Sound 2D entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& Sound2DEntity::GetScale() const NOEXCEPT
{
	//Sound 2D entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void Sound2DEntity::Move(const Vector3 &moveVector) NOEXCEPT
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
void Sound2DEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void Sound2DEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

#if defined(CATALYST_WINDOWS)
/*
*	Initializes this sound 2D entity.
*/
void Sound2DEntity::Initialize(const FMOD::Studio::EventDescription *const RESTRICT eventDescription) NOEXCEPT
{
	//Initialize this sound 2D entity via the Sound System.
	SoundSystem::Instance->InitializeSound2DEntity(this, eventDescription);
}
#endif