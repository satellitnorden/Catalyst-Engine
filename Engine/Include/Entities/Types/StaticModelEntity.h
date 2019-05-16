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
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Returns the material.
	*/
	RESTRICTED NO_DISCARD Material *const RESTRICT GetMaterial() NOEXCEPT;

};