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
*	Initializes the entity system.
*/
void EntitySystem::InitializeSystem() CATALYST_NOEXCEPT
{

}

/*
*	Updates the entity system synchronously.
*/
void EntitySystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
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
		delete Entity::entities[i];
	}
}