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
	*	Initializes this Maxim object.
	*/
	void Initialize(const float initialSpeed) NOEXCEPT;

	/*
	*	Pre-updates this Maxim object.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

private:

	//The speed.
	float _Speed;

	//The rotation.
	float _Rotation;

	//The entity.
	DynamicPhysicalEntity *RESTRICT _Entity;

};