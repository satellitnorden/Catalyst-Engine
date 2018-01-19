#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class Entity;

class EntitySystem
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntitySystem);

	/*
	*	Default constructor.
	*/
	EntitySystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~EntitySystem() CATALYST_NOEXCEPT;

	/*
	*	Releases the entity system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Creates a new entity.
	*/
	template <class EntityClass>
	CATALYST_RESTRICTED EntityClass* CreateEntity() const CATALYST_NOEXCEPT;

	/*
	*	Creates a new child entity.
	*/
	template <class EntityClass>
	CATALYST_RESTRICTED EntityClass* CreateChildEntity(Entity *CATALYST_RESTRICT parentEntity) const CATALYST_NOEXCEPT;

};

//////////////////////////////////////////////////////////////////////
//////////			Templated function definitions.			//////////
//////////////////////////////////////////////////////////////////////

/*
*	Creates a new entity.
*/
template <class EntityClass>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateEntity() const CATALYST_NOEXCEPT
{
	return new EntityClass();
}

/*
*	Creates a new child entity.
*/
template <class EntityClass>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateChildEntity(Entity *CATALYST_RESTRICT parentEntity) const CATALYST_NOEXCEPT
{
	EntityClass *CATALYST_RESTRICT newChild = new EntityClass();

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	return newChild;
}