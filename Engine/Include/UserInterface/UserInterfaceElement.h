#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Any.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>

//Forward declarations.
class UserInterfaceScene;

class UserInterfaceElement
{

public:

	//Constants.
	constexpr static uint64 USER_DATA_SIZE{ 8 };

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~UserInterfaceElement() NOEXCEPT
	{

	}

	/*
	*	Returns the parent.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD UserInterfaceScene *const RESTRICT GetParent() NOEXCEPT
	{
		return _Parent;
	}

	/*
	*	Sets the parent.
	*/
	FORCE_INLINE void SetParent(UserInterfaceScene *const RESTRICT value) NOEXCEPT
	{
		_Parent = value;
	}

	/*
	*	Returns the user data.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Any<USER_DATA_SIZE> *const RESTRICT GetUserData() NOEXCEPT
	{
		return &_UserData;
	}

	/*
	*	Returns the user interface primitives.
	*/
	FORCE_INLINE virtual void RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT = 0;

private:

	//The parent.
	UserInterfaceScene *RESTRICT _Parent;

	//The user data.
	Any<USER_DATA_SIZE> _UserData;

};