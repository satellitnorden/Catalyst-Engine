#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

class UserInterfaceSceneFactory final
{

public:

	//Type aliases.
	using CreateCallback = UserInterfaceScene *const RESTRICT(*)();
	using DestroyCallback = void(*)(UserInterfaceScene *const RESTRICT);

	/*
	*	Initializes this user interface scene factory.
	*/
	FORCE_INLINE void Initialize(	const char *const RESTRICT initial_name,
									const CreateCallback initial_create_callback,
									const DestroyCallback initial_destroy_callback) NOEXCEPT
	{
		_Name = initial_name;
		_Identifier = HashString(initial_name);
		_CreateCallback = initial_create_callback;
		_DestroyCallback = initial_destroy_callback;
	}

	/*
	*	Returns the name.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT GetName() const NOEXCEPT
	{
		return _Name.Data();
	}

	/*
	*	Returns the identifier.
	*/
	FORCE_INLINE NO_DISCARD HashString GetIdentifier() const NOEXCEPT
	{
		return _Identifier;
	}

	/*
	*	Creates a user interface scene.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD UserInterfaceScene *const RESTRICT Create() const NOEXCEPT
	{
		return _CreateCallback();
	}

	/*
	*	Destroys a user interface scene.
	*/
	FORCE_INLINE void Destroy(UserInterfaceScene *const RESTRICT scene) const NOEXCEPT
	{
		_DestroyCallback(scene);
	}

private:

	//The name.
	DynamicString _Name;

	//The identifier.
	HashString _Identifier;

	//The create callback.
	CreateCallback _CreateCallback;

	//The destroy callback.
	DestroyCallback _DestroyCallback;
	
};