#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class UserInterfaceScene
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~UserInterfaceScene() = default;

	/*
	*	Returns whether or not this scene is active.
	*/
	FORCE_INLINE NO_DISCARD bool GetIsActive() const NOEXCEPT
	{
		return _IsActive;
	}

	/*
	*	Sets whether or not this scene is active.
	*/
	FORCE_INLINE void SetIsActive(const bool value) NOEXCEPT
	{
		_IsActive = value;
	}

	/*
	*	Callback for when this user interface scene is activated.
	*/
	FORCE_INLINE virtual void OnActivated() NOEXCEPT
	{

	}

	/*
	*	Callback for when this user interface scene is activated updated
	*/
	FORCE_INLINE virtual void Update() NOEXCEPT
	{

	}

	/*
	*	Callback for when this user interface scene is deactivated.
	*/
	virtual void OnDeactivated() NOEXCEPT;

protected:

	/*
	*	Adds a button on the given row/column.
	*/
	void AddButton(	const uint32 vertical_index,
					const uint32 horizontal_index,
					const uint32 number_of_elements_vertically,
					const ButtonUserInterfaceElementCallback start_pressed_callback,
					const char *const RESTRICT text,
					UserInterfaceElement *RESTRICT *const RESTRICT button_element = nullptr,
					UserInterfaceElement *RESTRICT *const RESTRICT text_element = nullptr) NOEXCEPT;

private:

	//Denotes whether or not this scene is active.
	bool _IsActive{ false };

	//Container for all the user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

	/*
	*	Calculates the bounding box for the given row/column.
	*/
	void CalculateBoundingBox(	const uint32 vertical_index,
								const uint32 horizontal_index,
								const uint32 number_of_elements_vertically,
								Vector2<uint32> *const RESTRICT minimum,
								Vector2<uint32> *const RESTRICT maximum) NOEXCEPT;

};