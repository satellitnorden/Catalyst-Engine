//Header file.
#include <Entity.h>

//Systems.
#include <EntitySystem.h>
#include <PhysicsSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(Entity);

/*
*	Default constructor.
*/
Entity::Entity() CATALYST_NOEXCEPT
{
	//Add this entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
Entity::~Entity() CATALYST_NOEXCEPT
{
	//Remove this entity from the universal container.
	Instances.Erase(this);
}

/*
*	Marks this entitiy for destruction.
*/
void Entity::MarkForDestruction() CATALYST_NOEXCEPT
{

}