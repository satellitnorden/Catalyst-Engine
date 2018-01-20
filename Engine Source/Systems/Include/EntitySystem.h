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
	*	Pre-updates the entity system synchronously.
	*/
	void PreUpdateSystemSynchronous() CATALYST_NOEXCEPT;

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

	/*
	*	Marks a given entity for destruction.
	*/
	void MarkForDestruction(Entity *CATALYST_RESTRICT entityToBeDestroyed) CATALYST_NOEXCEPT;

private:

	//Container for all entities that are marked for destruction.
	DynamicArray<Entity *CATALYST_RESTRICT>entitiesToBeDestroyed;

};

//Templated function definitions.
#include <EntitySystem.inl>