#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Collection.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>

class MaximObject final : public Collection<MaximObject>
{

public:

	/*
	*	Default constructor.
	*/
	MaximObject() NOEXCEPT;

	/*
	*	Pre-updates this Maxim object.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

private:

	//The entity.
	DynamicPhysicalEntity *RESTRICT _Entity;

};