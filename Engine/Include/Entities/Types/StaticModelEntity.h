#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Rendering.
#include <Rendering/Native/Material.h>

class StaticModelEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	StaticModelEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the material.
	*/
	RESTRICTED NO_DISCARD Material *const RESTRICT GetMaterial() NOEXCEPT;

};