#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/UserInterfaceElement.h>

class UserInterfaceButton final : public UserInterfaceElement
{

public:

	//Type aliases.
	using Callback = void(*)(UserInterfaceButton *const RESTRICT button);

	/*
	*	Default constructor.
	*/
	UserInterfaceButton(const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const Callback initial_start_pressed_callback,
						const Callback initial_stop_pressed_callback,
						const Callback initial_start_hovered_callback,
						const Callback initial_stop_hovered_callback,
						const UserInterfaceMaterial &initial_idle_material,
						const UserInterfaceMaterial &initial_hovered_material,
						const UserInterfaceMaterial &initial_pressed_material,
						const char *const RESTRICT text,
						const AssetPointer<FontAsset> font,
						const float32 text_scale,
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
	*	Returns the idle material.
	*/
	FORCE_INLINE NO_DISCARD const UserInterfaceMaterial &GetIdleMaterial() const NOEXCEPT
	{
		return _IdleMaterial;
	}

	/*
	*	Sets the idle material.
	*/
	void SetIdleMaterial(const UserInterfaceMaterial& value) NOEXCEPT;

	/*
	*	Returns the hovered material.
	*/
	FORCE_INLINE NO_DISCARD const UserInterfaceMaterial &GetHoveredMaterial() const NOEXCEPT
	{
		return _HoveredMaterial;
	}

	/*
	*	Sets the hovered material.
	*/
	void SetHoveredMaterial(const UserInterfaceMaterial& value) NOEXCEPT;

	/*
	*	Returns the pressed material.
	*/
	FORCE_INLINE NO_DISCARD const UserInterfaceMaterial &GetPressedMaterial() const NOEXCEPT
	{
		return _PressedMaterial;
	}

	/*
	*	Sets the pressed material.
	*/
	void SetPressedMaterial(const UserInterfaceMaterial& value) NOEXCEPT;

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

	/*
	*	Sets the opacity.
	*/
	void SetOpacity(const float32 value) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT override;

private:

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

	//The current state.
	UserInterfaceButtonState _CurrentState;

	//The start pressed callback.
	Callback _StartPressedCallback;

	//The stop pressed callback.
	Callback _StopPressedCallback;

	//The start hovered callback.
	Callback _StartHoveredCallback;

	//The stop hovered callback.
	Callback _StopHoveredCallback;

	//The idle material.
	UserInterfaceMaterial _IdleMaterial;

	//The hovered material.
	UserInterfaceMaterial _HoveredMaterial;

	//The pressed material.
	UserInterfaceMaterial _PressedMaterial;

	//The font.
	AssetPointer<FontAsset> _Font;

	//Denotes whether or not this button is gamepad selected.
	bool _GamepadSelected{ false };

	//The text scale.
	float32 _TextScale;

	//The image primitive.
	ImageUserInterfacePrimitive *RESTRICT _ImagePrimitive;

	//The text primitive.
	TextUserInterfacePrimitive *RESTRICT _TextPrimitive{ nullptr };

	//Denotes whether or not this button is three dimensional.
	bool _IsThreeDimensional{ false };

};