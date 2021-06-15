#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>

class UserInterfaceCheckbox final
{

public:

	//Type aliases.
	using Callback = void(*)(UserInterfaceCheckbox *const RESTRICT checkbox);

	/*
	*	Default constructor.
	*/
	UserInterfaceCheckbox(	const Vector2<float32> initial_minimum,
							const Vector2<float32> initial_maximum,
							const Callback initial_start_hovered_callback,
							const Callback initial_stop_hovered_callback,
							const Callback initial_start_pressed_callback,
							const Callback initial_stop_pressed_callback,
							const UserInterfaceMaterial &initial_unchecked_idle_material,
							const UserInterfaceMaterial &initial_unchecked_hovered_material,
							const UserInterfaceMaterial &initial_unchecked_pressed_material,
							const UserInterfaceMaterial &initial_checked_idle_material,
							const UserInterfaceMaterial &initial_checked_hovered_material,
							const UserInterfaceMaterial &initial_checked_pressed_material,
							const char *const RESTRICT text,
							const ResourcePointer<FontResource> font_resource,
							const bool is_three_dimensional) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceCheckbox() NOEXCEPT;

	/*
	*	Returns the button minimum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetButtonMinimum() const NOEXCEPT
	{
		return _ButtonMinimum;
	}

	/*
	*	Returns the button maximum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetButtonMaximum() const NOEXCEPT
	{
		return _ButtonMaximum;
	}

	/*
	*	Returns the text minimum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetTextMinimum() const NOEXCEPT
	{
		return _TextMinimum;
	}

	/*
	*	Returns the text maximum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetTextMaximum() const NOEXCEPT
	{
		return _TextMaximum;
	}

	/*
	*	Returns whether or not this checkbox is checked.
	*/
	FORCE_INLINE NO_DISCARD bool GetIsChecked() const NOEXCEPT
	{
		return _IsChecked;
	}

	/*
	*	Sets whether or not this checkbox is checked.
	*/
	void SetIsChecked(const bool value) NOEXCEPT;

	/*
	*	Returns the current state.
	*/
	FORCE_INLINE NO_DISCARD UserInterfaceButtonState GetCurrentState() const NOEXCEPT
	{
		return _CurrentState;
	}

	/*
	*	Sets the current state.
	*/
	void SetCurrentState(const UserInterfaceButtonState value) NOEXCEPT;

	/*
	*	On start hovered callback.
	*/
	FORCE_INLINE void OnStartHovered() NOEXCEPT
	{
		if (_StartHoveredCallback)
		{
			_StartHoveredCallback(this);
		}
	}

	/*
	*	On stop hovered callback.
	*/
	FORCE_INLINE void OnStopHovered() NOEXCEPT
	{
		if (_StopHoveredCallback)
		{
			_StopHoveredCallback(this);
		}
	}

	/*
	*	On start pressed callback.
	*/
	FORCE_INLINE void OnStartPressed() NOEXCEPT
	{
		if (_StartPressedCallback)
		{
			_StartPressedCallback(this);
		}

		SetIsChecked(!GetIsChecked());
	}

	/*
	*	On stop pressed callback.
	*/
	FORCE_INLINE void OnStopPressed() NOEXCEPT
	{
		if (_StopPressedCallback)
		{
			_StopPressedCallback(this);
		}
	}

	/*
	*	Returns whether or not this button is gamepad selected.
	*/
	FORCE_INLINE NO_DISCARD bool GetGamepadSelected() const NOEXCEPT
	{
		return _GamepadSelected;
	}

	/*
	*	Sets whether or not this button is gamepad selected.
	*/
	FORCE_INLINE void SetGamepadSelected(const bool value) NOEXCEPT
	{
		_GamepadSelected = value;
	}

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

private:

	//The button minimum.
	Vector2<float32> _ButtonMinimum;

	//The button maximum.
	Vector2<float32> _ButtonMaximum;

	//The text minimum.
	Vector2<float32> _TextMinimum;

	//The text maximum.
	Vector2<float32> _TextMaximum;

	//Denotes whether or not this checkbox is checked.
	bool _IsChecked{ false };

	//The current state.
	UserInterfaceButtonState _CurrentState;

	//The start hovered callback.
	Callback _StartHoveredCallback;

	//The stop hovered callback.
	Callback _StopHoveredCallback;

	//The start pressed callback.
	Callback _StartPressedCallback;

	//The stop pressed callback.
	Callback _StopPressedCallback;

	//The unchecked idle material.
	UserInterfaceMaterial _UncheckedIdleMaterial;

	//The unchecked hovered material.
	UserInterfaceMaterial _UncheckedHoveredMaterial;

	//The unchecked pressed material.
	UserInterfaceMaterial _UncheckedPressedMaterial;

	//The checked idle material.
	UserInterfaceMaterial _CheckedIdleMaterial;

	//The checked hovered material.
	UserInterfaceMaterial _CheckedHoveredMaterial;

	//The checked pressed material.
	UserInterfaceMaterial _CheckedPressedMaterial;

	//The font resource.
	ResourcePointer<FontResource> _FontResource;

	//Denotes whether or not this button is gamepad selected.
	bool _GamepadSelected{ false };

	//The image primitive.
	ImageUserInterfacePrimitive *RESTRICT _ImagePrimitive;

	//The text primitive.
	TextUserInterfacePrimitive *RESTRICT _TextPrimitive{ nullptr };

	//Denotes whether or not this checkbox is three dimensional.
	bool _IsThreeDimensional{ false };

	/*
	*	Updates the material.
	*/
	void UpdateMaterial() NOEXCEPT;

};