#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Collection.h>
#include <Core/General/Updateable.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>

//Maxim.
#include <Main/MaximCore.h>

class MaximObject final : public Collection<MaximObject>, public Updateable
{

public:

	//The speed.
	static float _Speed;

	//Denotes whether or not this Maxim object is destroyed.
	bool _IsDestroyed{ false };

	//Denotes whether or not this Maxim object is the one currentlly selected.
	bool _IsSelected{ false };

	//The color.
	MaximColor _Color;

	//The entity.
	DynamicPhysicalEntity *RESTRICT _Entity;

	/*
	*	Equal operator overload.
	*/
	bool operator==(const MaximObject &other)
	{
		return _Entity == other._Entity;
	}

	/*
	*	Initializes this Maxim object.
	*/
	void Initialize(const MaximColor initialColor, const Vector3& initialOutlineColor, const float initialSpeed) NOEXCEPT;

	/*
	*	Logic updates this Maxim object asynchronously.
	*/
	bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

	/*
	*	Returns the entity.
	*/
	RESTRICTED DynamicPhysicalEntity *const RESTRICT GetEntity()
	{
		return _Entity;
	}

	/*
	*	Returns the position for a selected Maxim object.
	*/
	void UpdateSelectedPosition() NOEXCEPT;

};