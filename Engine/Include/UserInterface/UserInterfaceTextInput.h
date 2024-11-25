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

class UserInterfaceTextInput final : public UserInterfaceElement
{

public:

	//Type aliases.
	using Callback = void(*)(UserInterfaceTextInput*const RESTRICT button);

	/*
	*	Default constructor.
	*/
	UserInterfaceTextInput(	const Vector2<float32> initial_minimum,
							const Vector2<float32> initial_maximum,
							const UserInterfaceMaterial &initial_idle_material,
							const UserInterfaceMaterial &initial_active_material,
							const UserInterfaceMaterial &initial_hovered_material,
							const UserInterfaceMaterial &initial_pressed_material,
							const AssetPointer<FontAsset> font,
							const float32 text_scale,
							const char *const RESTRICT prompt_text,
							const char *const RESTRICT text,
							const Callback callback) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceTextInput() NOEXCEPT;

	/*
	*	Updates this user interface element.
	*/
	void Update() NOEXCEPT;

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
	*	Returns the text.
	*/
	FORCE_INLINE NO_DISCARD const DynamicString &GetText() const NOEXCEPT
	{
		return _CurrentText;
	}

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

	/*
	*	Sets the text opacity.
	*/
	void SetTextOpacity(const float32 value) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT override;

private:

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

	//The idle material.
	UserInterfaceMaterial _IdleMaterial;

	//The active material.
	UserInterfaceMaterial _ActiveMaterial;

	//The hovered material.
	UserInterfaceMaterial _HoveredMaterial;

	//The pressed material.
	UserInterfaceMaterial _PressedMaterial;

	//The font.
	AssetPointer<FontAsset> _Font;

	//The text scale.
	float32 _TextScale;

	//The prompt text.
	DynamicString _PromptText;

	//The current text.
	DynamicString _CurrentText;

	//The image primitive.
	ImageUserInterfacePrimitive *RESTRICT _ImagePrimitive;

	//The text primitive.
	TextUserInterfacePrimitive *RESTRICT _TextPrimitive{ nullptr };

	//The callback.
	Callback _Callback;

};