//Header file.
#include <Entities/ParticleSystemEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ParticleSystemEntity::ParticleSystemEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& ParticleSystemEntity::GetPosition() const NOEXCEPT
{
	//Particle system entities has no position.
	return Entity::_DefaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& ParticleSystemEntity::GetRotation() const NOEXCEPT
{
	//Particle system entities has no rotation.
	return Entity::_DefaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& ParticleSystemEntity::GetScale() const NOEXCEPT
{
	//Particle system entities has no scale.
	return Entity::_DefaultScale;
}

/*
*	Moves this entity.
*/
void ParticleSystemEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this particle system entity.
	ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._Properties.worldPosition += moveVector;

	//Move all children.
	for (auto child : _Children)
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
	for (auto child : _Children)
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
	for (auto child : _Children)
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