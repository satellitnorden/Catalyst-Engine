//Header file.
#include <Entities/StaticPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
StaticPhysicalEntity::StaticPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewStaticPhysicalComponentsIndex(this);
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
NO_DISCARD RESTRICTED Vector3 *const RESTRICT StaticPhysicalEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT StaticPhysicalEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT StaticPhysicalEntity::GetScaleInternal() NOEXCEPT
{
	//Return the scale of this entity.
	return &ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Scale;
}