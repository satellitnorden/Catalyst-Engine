//Header file.
#include <Entities/Entity.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(Entity);

//Static variable definitions.
constexpr Vector3 Entity::defaultPosition;
constexpr Vector3 Entity::defaultRotation;
constexpr Vector3 Entity::defaultScale;

/*
*	Default constructor.
*/
Entity::Entity() NOEXCEPT
{
	//Add this entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
Entity::~Entity() NOEXCEPT
{
	//Remove this entity from the universal container.
	Instances.Erase(this);
}

/*
*	Marks this entitiy for destruction.
*/
void Entity::MarkForDestruction() NOEXCEPT
{

}