//Header file.
#include <Entities/ParticleSystemEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(ParticleSystemEntity);

/*
*	Default constructor.
*/
ParticleSystemEntity::ParticleSystemEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex();

	//Add this particle system entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
ParticleSystemEntity::~ParticleSystemEntity() NOEXCEPT
{
	//Remove this particle system entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& ParticleSystemEntity::GetPosition() const NOEXCEPT
{
	//Particle system entities has no position.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& ParticleSystemEntity::GetRotation() const NOEXCEPT
{
	//Particle system entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& ParticleSystemEntity::GetScale() const NOEXCEPT
{
	//Particle system entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void ParticleSystemEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this particle system entity.
	ComponentManager::GetParticleSystemComponents()[componentsIndex].properties.worldPosition += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void ParticleSystemEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void ParticleSystemEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this particle system entity.
*/
void ParticleSystemEntity::Initialize(const ParticleMaterial &material, const ParticleSystemProperties &properties) NOEXCEPT
{
	//Initialize this particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(*this, material, properties);
}