#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Entities.
#include <Entities/Core/Entity.h>

class EntityLinks final
{

public:

	/*
	*	Updates the entity links.
	*/
	void Update() NOEXCEPT;

	/*
	*	Adds an entity link.
	*/
	void AddLink(Entity *const RESTRICT from, Entity *const RESTRICT to) NOEXCEPT;

	/*
	*	Removes the links for the given entity.
	*/
	void RemoveLinks(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Returns if the given entity has links.
	*/
	NO_DISCARD bool HasLinks(Entity *const RESTRICT entity) const NOEXCEPT;

	/*
	*	Returns the links for the given entity.
	*/
	NO_DISCARD const DynamicArray<Entity *RESTRICT> &GetLinks(Entity *const RESTRICT entity) const NOEXCEPT;

private:

	/*
	*	Add link request class definition.
	*/
	class AddLinkRequest final
	{

	public:

		//The from entity.
		Entity *RESTRICT _From;

		//The to entity.
		Entity *RESTRICT _To;

	};

	/*
	*	Remove links request class definition.
	*/
	class RemoveLinksRequest final
	{

	public:

		//The entity.
		Entity *RESTRICT _Entity;

	};

	//The add link queue.
	AtomicQueue<AddLinkRequest, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _AddLinkQueue;

	//The remove links queue.
	AtomicQueue<RemoveLinksRequest, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _RemoveLinksQueue;

	//The entity to links mappings.
	DynamicArray<uint64> _EntityToLinksMappings;

	//The links.
	DynamicArray<DynamicArray<Entity *RESTRICT>> _Links;

	/*
	*	Adds an entity link internally.
	*/
	void AddLinkInternal(Entity *const RESTRICT from, Entity *const RESTRICT to) NOEXCEPT;

	/*
	*	Removes the links for the given entity internally.
	*/
	void RemoveLinksInternal(Entity *const RESTRICT entity) NOEXCEPT;

};