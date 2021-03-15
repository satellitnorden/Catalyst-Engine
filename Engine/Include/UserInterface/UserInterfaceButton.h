#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElement.h>
#include <UserInterface/TextUserInterfaceElement.h>

class UserInterfaceButton final
{

public:

	//Enumeration covering all states.
	enum class State : uint8
	{
		IDLE,
		HOVERED,
		PRESSED
	};

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
						const char *const RESTRICT text) NOEXCEPT;

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
	FORCE_INLINE NO_DISCARD State GetCurrentState() const NOEXCEPT
	{
		return _CurrentState;
	}

	/*
	*	Sets the current state.
	*/
	FORCE_INLINE void SetCurrentState(const State value) NOEXCEPT
	{
		_CurrentState = value;
	}

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
	State _CurrentState;

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

	//Denotes whether or not this button is gamepad selected.
	bool _GamepadSelected{ false };

	//The button element.
	ButtonUserInterfaceElement *RESTRICT _ButtonElement;

	//The text element.
	TextUserInterfaceElement *RESTRICT _TextElement;

};