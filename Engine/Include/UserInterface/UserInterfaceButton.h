#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>

class UserInterfaceButton final
{

public:

	//Type aliases.
	using Callback = void(*)(UserInterfaceButton *const RESTRICT button);

	/*
	*	Default constructor.
	*/
	UserInterfaceButton(const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const Callback initial_start_hovered_callback,
						const Callback initial_stop_hovered_callback,
						const Callback initial_start_pressed_callback,
						const Callback initial_stop_pressed_callback,
						const UserInterfaceMaterial &initial_idle_material,
						const UserInterfaceMaterial &initial_hovered_material,
						const UserInterfaceMaterial &initial_pressed_material,
						const char *const RESTRICT text,
						const ResourcePointer<FontResource> font_resource,
						const bool is_three_dimensional) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceButton() NOEXCEPT;

	/*
	*	Returns the minimum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetMinimum() const NOEXCEPT
	{
		return _Minimum;
	}

	/*
	*	Returns the maximum.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> GetMaximum() const NOEXCEPT
	{
		return _Maximum;
	}

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

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

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

	//The idle material.
	UserInterfaceMaterial _IdleMaterial;

	//The hovered material.
	UserInterfaceMaterial _HoveredMaterial;

	//The pressed material.
	UserInterfaceMaterial _PressedMaterial;

	//The font resource.
	ResourcePointer<FontResource> _FontResource;

	//Denotes whether or not this button is gamepad selected.
	bool _GamepadSelected{ false };

	//The image primitive.
	ImageUserInterfacePrimitive *RESTRICT _ImagePrimitive;

	//The text primitive.
	TextUserInterfacePrimitive *RESTRICT _TextPrimitive{ nullptr };

	//Denotes whether or not this button is three dimensional.
	bool _IsThreeDimensional{ false };

};