#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Collection.h>
#include <Core/General/Updateable.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>

class MaximObject final : public Collection<MaximObject>, public Updateable
{

public:

	/*
	*	Equal operator overload.
	*/
	bool operator==(const MaximObject &other)
	{
		return	_Speed == other._Speed
				&& _Rotation == other._Rotation
				&& _Entity == other._Entity;
	}

	/*
	*	Initializes this Maxim object.
	*/
	void Initialize(const float initialSpeed) NOEXCEPT;

	/*
	*	Pre-updates this Maxim object asynchronously.
	*/
	bool PreUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

	/*
	*	Returns the entity.
	*/
	RESTRICTED DynamicPhysicalEntity *const RESTRICT GetEntity()
	{
		return _Entity;
	}

private:

	//The speed.
	float _Speed;

	//The rotation.
	float _Rotation;

	//The entity.
	DynamicPhysicalEntity *RESTRICT _Entity;

};