//Header file.
#include <Entity.h>

//Systems.
#include <EntitySystem.h>
#include <PhysicsSystem.h>

//Static variable definitions.
DynamicArray<Entity *CATALYST_RESTRICT> Entity::instances;

/*
*	Default constructor.
*/
Entity::Entity() CATALYST_NOEXCEPT
{
	//Add this entity to the universal container.
	instances.Emplace(this);
}

/*
*	Default destructor.
*/
Entity::~Entity() CATALYST_NOEXCEPT
{
	//Remove this entity from the universal container.
	instances.Erase(this);
}

/*
*	Marks this entitiy for destruction.
*/
void Entity::MarkForDestruction() CATALYST_NOEXCEPT
{

}