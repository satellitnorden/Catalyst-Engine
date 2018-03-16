//Header file.
#include <Systems/EntitySystem.h>

//Entities.
#include <Entities/Entity.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Default constructor.
*/
EntitySystem::EntitySystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
EntitySystem::~EntitySystem() NOEXCEPT
{

}

/*
*	Releases the entity system.
*/
void EntitySystem::ReleaseSystem() NOEXCEPT
{
	//Destroy all remaining entities.
	for (int64 i = Entity::Instances.Size() - 1; i >= 0; --i)
	{
		delete Entity::Instances[i];
	}
}

/*
*	Updates the entity system synchronously.
*/
void EntitySystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Destroy all entities that are marked to be destroyed.
	for (int64 i = entitiesToBeDestroyed.Size() - 1; i >= 0; --i)
	{
		delete entitiesToBeDestroyed[i];
	}
}

/*
*	Marks a given entity for destruction.
*/
void EntitySystem::MarkForDestruction(Entity *const RESTRICT entityToBeDestroyed) NOEXCEPT
{
	entitiesToBeDestroyed.EmplaceSlow(entityToBeDestroyed);
}