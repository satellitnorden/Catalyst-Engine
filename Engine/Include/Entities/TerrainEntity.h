#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class TerrainEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	TerrainEntity() NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

};