//Header file.
#include <Entities/Core/EntityLinks.h>

/*
*	Updates the entity links.
*/
void EntityLinks::Update() NOEXCEPT
{
	//Process the remove links queue.
	{
		Optional<RemoveLinksRequest> request{ _RemoveLinksQueue.Pop() };

		while (request.Valid())
		{
			RemoveLinksInternal(request.Get()._Entity);

			request = _RemoveLinksQueue.Pop();
		}
	}

	//Process the add link queue.
	{
		Optional<AddLinkRequest> request{ _AddLinkQueue.Pop() };

		while (request.Valid())
		{
			AddLinkInternal(request.Get()._From, request.Get()._To);

			request = _AddLinkQueue.Pop();
		}
	}
}

/*
*	Adds an entity link.
*/
void EntityLinks::AddLink(Entity *const RESTRICT from, Entity *const RESTRICT to) NOEXCEPT
{
	//Create the request.
	AddLinkRequest request;

	request._From = from;
	request._To = to;

	_AddLinkQueue.Push(request);
}

/*
*	Removes the links for the given entity.
*/
void EntityLinks::RemoveLinks(Entity *const RESTRICT entity) NOEXCEPT
{
	//Create the request.
	RemoveLinksRequest request;

	request._Entity = entity;

	_RemoveLinksQueue.Push(request);
}

/*
*	Returns if the given entity has links.
*/
NO_DISCARD bool EntityLinks::HasLinks(Entity *const RESTRICT entity) const NOEXCEPT
{
	return	entity->_EntityIdentifier < _EntityToLinksMappings.Size()
			&& _EntityToLinksMappings[entity->_EntityIdentifier] != UINT64_MAXIMUM
			&& !_Links[_EntityToLinksMappings[entity->_EntityIdentifier]].Empty(); 
}

/*
*	Returns the links for the given entity.
*/
NO_DISCARD const DynamicArray<Entity *RESTRICT> &EntityLinks::GetLinks(Entity *const RESTRICT entity) const NOEXCEPT
{
	const static DynamicArray<Entity *RESTRICT> EMPTY;

	if (HasLinks(entity))
	{
		return _Links[_EntityToLinksMappings[entity->_EntityIdentifier]];
	}
	
	else
	{
		return EMPTY;
	}
}

/*
*	Adds an entity link internally.
*/
void EntityLinks::AddLinkInternal(Entity *const RESTRICT from, Entity *const RESTRICT to) NOEXCEPT
{
	//Add entity to links mappings up to this entity.
	for (uint64 i{ _EntityToLinksMappings.Size() }; i <= from->_EntityIdentifier; ++i)
	{
		_EntityToLinksMappings.Emplace(UINT64_MAXIMUM);
	}

	//Cache the links index.
	const uint64 links_index{ _EntityToLinksMappings[from->_EntityIdentifier] };

	//If this entity already has links, just add it to that.
	if (links_index != UINT64_MAXIMUM)
	{
		_Links[links_index].Emplace(to);
	}

	//Otherwise, create new links.
	else
	{
		//Add the links.
		_Links.Emplace();
		DynamicArray<Entity *RESTRICT> &links{ _Links.Back() };

		//Add the link.
		links.Emplace(to);

		//Update the entity to links mapping.
		_EntityToLinksMappings[from->_EntityIdentifier] = _Links.LastIndex();
	}
}

/*
*	Removes the links for the given entity internally.
*/
void EntityLinks::RemoveLinksInternal(Entity *const RESTRICT entity) NOEXCEPT
{
	if (HasLinks(entity))
	{
		_Links[_EntityToLinksMappings[entity->_EntityIdentifier]].Destroy();
	}
}