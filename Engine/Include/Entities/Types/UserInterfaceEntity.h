#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldTransform.h>

class UserInterfaceEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return false;
	}

	/*
	*	Returns the initialization data required to duplicate this entity.
	*/
	RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT;

	/*
	*	Returns the user interface scene.
	*/
	RESTRICTED NO_DISCARD const UserInterfaceScene *const RESTRICT GetUserInterfaceScene() const NOEXCEPT;

	/*
	*	Sets the user interface scene.
	*/
	void SetUserInterfaceScene(UserInterfaceScene *const RESTRICT value) NOEXCEPT;

	/*
	*	Returns the world transform.
	*/
	NO_DISCARD const WorldTransform &GetWorldTransform() const NOEXCEPT;

	/*
	*	Sets the world transform.
	*/
	void SetWorldTransform(const WorldTransform &value) NOEXCEPT;

};