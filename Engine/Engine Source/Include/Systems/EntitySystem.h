#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
class Entity;

class EntitySystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntitySystem);

	/*
	*	Pre-updates the entity system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the entity system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Creates a new entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* CreateEntity(Arguments&&... arguments) const NOEXCEPT;

	/*
	*	Creates a new child entity.
	*/
	template <class EntityClass, class... Arguments>
	RESTRICTED EntityClass* CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) const NOEXCEPT;

	/*
	*	Marks a given entity for destruction.
	*/
	void MarkForDestruction(Entity *RESTRICT entityToBeDestroyed) NOEXCEPT;

private:

	//Container for all entities that are marked for destruction.
	DynamicArray<Entity *RESTRICT>entitiesToBeDestroyed;

};

//Templated function definitions.
#include <Systems/EntitySystem.inl>