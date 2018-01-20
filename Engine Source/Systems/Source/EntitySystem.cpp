//Header file.
#include <EntitySystem.h>

//Entities.
#include <Entity.h>

//Singleton definition.
DEFINE_SINGLETON(EntitySystem);

/*
*	Default constructor.
*/
EntitySystem::EntitySystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
EntitySystem::~EntitySystem() CATALYST_NOEXCEPT
{

}

/*
*	Releases the entity system.
*/
void EntitySystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	//Destroy all remaining entities.
	for (int64 i = Entity::entities.Size() - 1; i >= 0; --i)
	{
		Entity::entities[i]->Destroy();
		delete Entity::entities[i];
	}
}

/*
*	Pre-updates the entity system synchronously.
*/
void EntitySystem::PreUpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Destroy all entities that are marked to be destroyed.
	for (int64 i = entitiesToBeDestroyed.Size() - 1; i >= 0; --i)
	{
		entitiesToBeDestroyed[i]->Destroy();
		delete entitiesToBeDestroyed[i];
	}
}

/*
*	Marks a given entity for destruction.
*/
void EntitySystem::MarkForDestruction(Entity *const CATALYST_RESTRICT entityToBeDestroyed) CATALYST_NOEXCEPT
{
	entitiesToBeDestroyed.Emplace(entityToBeDestroyed);
}